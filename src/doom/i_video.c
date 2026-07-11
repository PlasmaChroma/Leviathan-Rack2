#include <stdlib.h>
#include <string.h>
#include "doomtype.h"
#include "i_video.h"

char *video_driver = "headless";
boolean screenvisible = true;
boolean screensaver_mode = false;
int usegamma = 0;
pixel_t *I_VideoBuffer = NULL;

int screen_width = SCREENWIDTH;
int screen_height = SCREENHEIGHT;
int fullscreen = 0;
int aspect_ratio_correct = 1;
int integer_scaling = 0;
int vga_porch_flash = 0;
int force_software_renderer = 1;
char *window_position = "center";
unsigned int joywait = 0;

void I_GetWindowPosition(int *x, int *y, int w, int h)
{
    if (x) *x = 0;
    if (y) *y = 0;
}

void I_InitGraphics(void)
{
    I_VideoBuffer = malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(pixel_t));
    memset(I_VideoBuffer, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(pixel_t));
}

void I_GraphicsCheckCommandLine(void)
{
}

void I_ShutdownGraphics(void)
{
    if (I_VideoBuffer)
    {
        free(I_VideoBuffer);
        I_VideoBuffer = NULL;
    }
}

static byte active_palette[768];
static uint8_t *doom_rgba_buffer = NULL;
volatile int doom_dirty_frame = 0;
int doom_exit_requested = 0;
int usemouse = 1;
volatile int g_game_health = 100;
volatile int g_game_frag_trigger = 0;
volatile float g_cv_xmove = 0.0f;
volatile float g_cv_ymove = 0.0f;
volatile int g_cv_fire = 0;
volatile int g_cv_weapon = -1;

void I_SetPalette(byte* palette)
{
    memcpy(active_palette, palette, 768);
}

int I_GetPaletteIndex(int r, int g, int b)
{
    return 0;
}

void I_UpdateNoBlit(void)
{
}

void I_FinishUpdate(void)
{
    if (doom_rgba_buffer && I_VideoBuffer)
    {
        for (int i = 0; i < 320 * 200; ++i)
        {
            byte pixel = I_VideoBuffer[i];
            doom_rgba_buffer[i * 4 + 0] = active_palette[pixel * 3 + 0]; // R
            doom_rgba_buffer[i * 4 + 1] = active_palette[pixel * 3 + 1]; // G
            doom_rgba_buffer[i * 4 + 2] = active_palette[pixel * 3 + 2]; // B
            doom_rgba_buffer[i * 4 + 3] = 255;                          // A
        }
        doom_dirty_frame = 1;
    }
}

void I_SetTargetRGBA(uint8_t *buffer)
{
    doom_rgba_buffer = buffer;
}

void I_ReadScreen(pixel_t* scr)
{
    if (scr && I_VideoBuffer)
    {
        memcpy(scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT * sizeof(pixel_t));
    }
}

void I_BeginRead(void)
{
}

void I_SetWindowTitle(char *title)
{
}

void I_CheckIsScreensaver(void)
{
}

void I_SetGrabMouseCallback(grabmouse_callback_t func)
{
}

void I_DisplayFPSDots(boolean dots_on)
{
}

void I_BindVideoVariables(void)
{
}

void I_InitWindowTitle(void)
{
}

void I_InitWindowIcon(void)
{
}

void I_StartFrame(void)
{
}

void I_StartTic(void)
{
}

void I_EnableLoadingDisk(int xoffs, int yoffs)
{
}
