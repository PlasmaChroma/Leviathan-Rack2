#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

// VCV Doom runs the engine in its own thread on every platform.
#include <pthread.h>

#include "config.h"
#include "deh_str.h"
#include "doomtype.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "i_joystick.h"
#include "i_sound.h"
#include "i_timer.h"
#include "i_video.h"
#include "i_system.h"
#include "w_wad.h"
#include "z_zone.h"

#define DEFAULT_RAM 16 /* MiB */
#define MIN_RAM     4  /* MiB */

// Exposed to the Rack module so startup failures in the headless engine do
// not disappear into the host process' stderr stream.
volatile int doom_engine_status = 0;
char doom_engine_error[256] = "";

typedef struct atexit_listentry_s atexit_listentry_t;

struct atexit_listentry_s
{
    atexit_func_t func;
    boolean run_on_error;
    atexit_listentry_t *next;
};

static atexit_listentry_t *exit_funcs = NULL;

void I_AtExit(atexit_func_t func, boolean run_on_error)
{
    atexit_listentry_t *entry;

    entry = malloc(sizeof(*entry));

    entry->func = func;
    entry->run_on_error = run_on_error;
    entry->next = exit_funcs;
    exit_funcs = entry;
}

void I_Tactile(int on, int off, int total)
{
}

static byte *AutoAllocMemory(int *size, int default_ram, int min_ram)
{
    byte *zonemem;

    zonemem = NULL;

    while (zonemem == NULL)
    {
        if (default_ram < min_ram)
        {
            I_Error("Unable to allocate %i MiB of RAM for zone", default_ram);
        }

        *size = default_ram * 1024 * 1024;
        zonemem = malloc(*size);

        if (zonemem == NULL)
        {
            default_ram -= 1;
        }
    }

    return zonemem;
}

byte *I_ZoneBase (int *size)
{
    byte *zonemem;
    int min_ram, default_ram;
    int p;

    p = M_CheckParmWithArgs("-mb", 1);

    if (p > 0)
    {
        default_ram = atoi(myargv[p+1]);
        min_ram = default_ram;
    }
    else
    {
        default_ram = DEFAULT_RAM;
        min_ram = MIN_RAM;
    }

    zonemem = AutoAllocMemory(size, default_ram, min_ram);

    printf("zone memory: %p, %x allocated for zone\n", 
           zonemem, *size);

    return zonemem;
}

void I_PrintBanner(char *msg)
{
    int i;
    int spaces = 35 - (strlen(msg) / 2);

    for (i=0; i<spaces; ++i)
        putchar(' ');

    puts(msg);
}

void I_PrintDivider(void)
{
    int i;

    for (i=0; i<75; ++i)
    {
        putchar('=');
    }

    putchar('\n');
}

void I_PrintStartupBanner(char *gamedescription)
{
    I_PrintDivider();
    I_PrintBanner(gamedescription);
    I_PrintDivider();
    
    printf(
    " " PACKAGE_NAME " is free software, covered by the GNU General Public\n"
    " License.  There is NO warranty; not even for MERCHANTABILITY or FITNESS\n"
    " FOR A PARTICULAR PURPOSE. You are welcome to change and distribute\n"
    " copies under certain conditions. See the source for more information.\n");

    I_PrintDivider();
}

boolean I_ConsoleStdout(void)
{
#ifdef _WIN32
    return false;
#else
    return isatty(fileno(stdout));
#endif
}

void I_Init (void)
{
    I_InitTimer();
    I_InitJoystick();
}

void I_BindVariables(void)
{
    I_BindVideoVariables();
    I_BindJoystickVariables();
    I_BindSoundVariables();
}

static boolean already_quitting = false;

void I_Quit (void)
{
    atexit_listentry_t *entry;
    atexit_listentry_t *next;

    entry = exit_funcs; 
    exit_funcs = NULL;

    while (entry != NULL)
    {
        entry->func();
        next = entry->next;
        free(entry);
        entry = next;
    }

    already_quitting = false;
    pthread_exit(NULL);
}

void I_Error (char *error, ...)
{
    va_list argptr;
    va_list error_copy;
    atexit_listentry_t *entry;
    atexit_listentry_t *next;

    if (already_quitting)
    {
        fprintf(stderr, "Warning: recursive call to I_Error detected.\n");
        pthread_exit(NULL);
    }
    else
    {
        already_quitting = true;
    }

    // Message first.
    va_start(argptr, error);
    va_copy(error_copy, argptr);
    vsnprintf(doom_engine_error, sizeof(doom_engine_error), error, error_copy);
    va_end(error_copy);
    doom_engine_status = -1;
    fprintf(stderr, "\nError: ");
    vfprintf(stderr, error, argptr);
    fprintf(stderr, "\n\n");
    va_end(argptr);
    fflush(stderr);

    entry = exit_funcs;
    exit_funcs = NULL;

    while (entry != NULL)
    {
        if (entry->run_on_error)
        {
            entry->func();
        }

        next = entry->next;
        free(entry);
        entry = next;
    }

    already_quitting = false;
    pthread_exit(NULL);
}

void *I_Realloc(void *ptr, size_t size)
{
    void *new_ptr;

    new_ptr = realloc(ptr, size);

    if (size != 0 && new_ptr == NULL)
    {
        I_Error ("I_Realloc: failed on reallocation of %li bytes", (long)size);
    }

    return new_ptr;
}

#define DOS_MEM_DUMP_SIZE 10

static const unsigned char mem_dump_dos622[DOS_MEM_DUMP_SIZE] = {
  0x57, 0x92, 0x19, 0x00, 0xF4, 0x06, 0x70, 0x00, 0x16, 0x00};
static const unsigned char mem_dump_win98[DOS_MEM_DUMP_SIZE] = {
  0x9E, 0x0F, 0xC9, 0x00, 0x65, 0x04, 0x70, 0x00, 0x16, 0x00};
static const unsigned char mem_dump_dosbox[DOS_MEM_DUMP_SIZE] = {
  0x00, 0x00, 0x00, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
static unsigned char mem_dump_custom[DOS_MEM_DUMP_SIZE];

static const unsigned char *dos_mem_dump = mem_dump_dos622;

boolean I_GetMemoryValue(unsigned int offset, void *value, int size)
{
    static boolean firsttime = true;

    if (firsttime)
    {
        int p, i, val;

        firsttime = false;
        i = 0;

        p = M_CheckParmWithArgs("-setmem", 1);

        if (p > 0)
        {
            if (!strcasecmp(myargv[p + 1], "dos622"))
            {
                dos_mem_dump = mem_dump_dos622;
            }
            if (!strcasecmp(myargv[p + 1], "dos71"))
            {
                dos_mem_dump = mem_dump_win98;
            }
            else if (!strcasecmp(myargv[p + 1], "dosbox"))
            {
                dos_mem_dump = mem_dump_dosbox;
            }
            else
            {
                for (i = 0; i < DOS_MEM_DUMP_SIZE; ++i)
                {
                    ++p;

                    if (p >= myargc || myargv[p][0] == '-')
                    {
                        break;
                    }

                    M_StrToInt(myargv[p], &val);
                    mem_dump_custom[i++] = (unsigned char) val;
                }

                dos_mem_dump = mem_dump_custom;
            }
        }
    }

    switch (size)
    {
    case 1:
        *((unsigned char *) value) = dos_mem_dump[offset];
        return true;
    case 2:
        *((unsigned short *) value) = dos_mem_dump[offset]
                                    | (dos_mem_dump[offset + 1] << 8);
        return true;
    case 4:
        *((unsigned int *) value) = dos_mem_dump[offset]
                                  | (dos_mem_dump[offset + 1] << 8)
                                  | (dos_mem_dump[offset + 2] << 16)
                                  | (dos_mem_dump[offset + 3] << 24);
        return true;
    }

    return false;
}
