#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doomtype.h"
#include "i_sound.h"
#include "w_wad.h"
#include "z_zone.h"
#include "midifile.h"
#include "mus2mid.h"
#include "memio.h"

volatile int g_music_playing = 0;
volatile int g_music_looping = 0;
volatile double g_music_ticks = 0.0;
volatile double g_music_ticks_per_sec = 0.0;
volatile unsigned int g_time_division = 0;
volatile unsigned int g_tempo = 500000;
volatile unsigned int g_next_event_tick = 0;
volatile unsigned int g_music_generation = 0;
void *g_active_midi_file = NULL;
void *g_active_midi_iter = NULL;

int snd_sfxdevice = SNDDEVICE_SB;
int snd_musicdevice = SNDDEVICE_ADLIB;
int snd_samplerate = 11025;
int snd_cachesize = 8 * 1024 * 1024;
int snd_maxslicetime_ms = 28;
char *snd_musiccmd = "";
int snd_pitchshift = 0;

mixer_channel_t g_mixer_channels[MIXER_CHANNELS];

void I_InitSound(boolean use_sfx_prefix)
{
    for (int i = 0; i < MIXER_CHANNELS; ++i)
    {
        g_mixer_channels[i].active = 0;
        g_mixer_channels[i].data = NULL;
        g_mixer_channels[i].capacity = 0;
        g_mixer_channels[i].length = 0;
        g_mixer_channels[i].pos = 0.0f;
    }
}

void I_ShutdownSound(void)
{
    for (int i = 0; i < MIXER_CHANNELS; ++i)
    {
        g_mixer_channels[i].active = 0;
        if (g_mixer_channels[i].data)
        {
            free(g_mixer_channels[i].data);
            g_mixer_channels[i].data = NULL;
            g_mixer_channels[i].capacity = 0;
        }
    }
}

int I_GetSfxLumpNum(sfxinfo_t *sfxinfo)
{
    char lumpname[9];

    // Doom's S_sfx table stores names without the WAD's "DS" prefix.
    // Return the real lump number so S_StartSound can cache it.
    snprintf(lumpname, sizeof(lumpname), "DS%.6s", sfxinfo->name);
    return W_GetNumForName(lumpname);
}

void I_UpdateSound(void)
{
}

void I_UpdateSoundParams(int channel, int vol, int sep)
{
    if (channel >= 0 && channel < MIXER_CHANNELS)
    {
        g_mixer_channels[channel].vol = vol;
        g_mixer_channels[channel].sep = sep;
    }
}

int I_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep, int pitch)
{
    if (channel < 0 || channel >= MIXER_CHANNELS)
        return 0;

    if (sfxinfo->lumpnum < 0)
        return 0;

    byte *data = (byte *)W_CacheLumpNum(sfxinfo->lumpnum, PU_STATIC);
    if (!data)
        return 0;

    // DMX sound lump header: format, sample rate, then sample count.
    const uint32_t lump_length = (uint32_t) W_LumpLength(sfxinfo->lumpnum);
    if (lump_length <= 8)
        return 0;

    int samplerate = (data[3] << 8) | data[2];
    uint32_t length = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];

    // Keep malformed headers from reading past the cached WAD lump.
    if (length > lump_length - 8)
        length = lump_length - 8;

    if (length == 0)
        return 0;

    mixer_channel_t *chan = &g_mixer_channels[channel];

    // Atomically deactivate before mutating
    chan->active = 0;

    // Check capacity
    if (length > chan->capacity)
    {
        uint8_t *new_buf = (uint8_t *)realloc(chan->data, length);
        if (!new_buf)
        {
            return 0;
        }
        chan->data = new_buf;
        chan->capacity = length;
    }

    // Copy sample data (skip 8-byte header)
    memcpy(chan->data, data + 8, length);
    chan->length = length;
    chan->vol = vol;
    chan->sep = sep;

    // Pitch calculation (127 is normal)
    float pitch_factor = (float)pitch / 127.0f;
    if (pitch_factor < 0.5f) pitch_factor = 0.5f;
    if (pitch_factor > 2.0f) pitch_factor = 2.0f;

    chan->src_rate = samplerate;
    chan->pitch_factor = pitch_factor;
    chan->pos = 0.0f;

    // Mark active
    chan->active = 1;

    // The Doom sound layer stores this handle and passes it unchanged to
    // I_UpdateSoundParams(), I_StopSound(), and I_SoundIsPlaying().
    return channel;
}

void I_StopSound(int channel)
{
    if (channel >= 0 && channel < MIXER_CHANNELS)
    {
        g_mixer_channels[channel].active = 0;
    }
}

boolean I_SoundIsPlaying(int channel)
{
    if (channel >= 0 && channel < MIXER_CHANNELS)
    {
        return g_mixer_channels[channel].active != 0;
    }
    return false;
}

void I_PrecacheSounds(sfxinfo_t *sounds, int num_sounds)
{
}

void I_InitMusic(void)
{
}

void I_ShutdownMusic(void)
{
}

void I_SetMusicVolume(int volume)
{
}

void I_PauseSong(void)
{
    g_music_playing = 0;
}

void I_ResumeSong(void)
{
    if (g_active_midi_file && g_active_midi_iter)
    {
        g_music_playing = 1;
    }
}

void *I_RegisterSong(void *data, int len)
{
    if (!data || len <= 0) return NULL;
    
    MEMFILE *mus_file = mem_fopen_read(data, len);
    if (!mus_file) return NULL;
    
    MEMFILE *mid_file = mem_fopen_write();
    if (!mid_file) {
        mem_fclose(mus_file);
        return NULL;
    }
    
    // mus2mid() follows the Doom convention: false means success.
    if (mus2mid(mus_file, mid_file)) {
        mem_fclose(mus_file);
        mem_fclose(mid_file);
        return NULL;
    }
    
    void *midi_buf = NULL;
    size_t midi_len = 0;
    mem_get_buf(mid_file, &midi_buf, &midi_len);
    
    midi_file_t *midi = MIDI_LoadFileFromMemory(midi_buf, midi_len);
    
    mem_fclose(mus_file);
    mem_fclose(mid_file);
    
    return (void *)midi;
}

void I_UnRegisterSong(void *handle)
{
    if (handle) {
        if (g_active_midi_file == handle) {
            I_StopSong();
        }
        MIDI_FreeFile((midi_file_t *)handle);
    }
}

void I_PlaySong(void *handle, boolean looping)
{
    if (!handle) return;
    
    g_music_playing = 0;
    if (g_active_midi_iter) {
        MIDI_FreeIterator((midi_track_iter_t *)g_active_midi_iter);
        g_active_midi_iter = NULL;
    }
    
    g_active_midi_file = handle;
    g_music_looping = looping;
    g_time_division = MIDI_GetFileTimeDivision((midi_file_t *)handle);
    g_music_ticks = 0.0;
    g_tempo = 500000;
    g_music_ticks_per_sec = (1000000.0 / g_tempo) * g_time_division;
    g_active_midi_iter = (void *)MIDI_IterateTrack((midi_file_t *)handle, 0);
    g_next_event_tick = MIDI_GetDeltaTime((midi_track_iter_t *)g_active_midi_iter);
    g_music_playing = 1;
    ++g_music_generation;
}

void I_StopSong(void)
{
    g_music_playing = 0;
    if (g_active_midi_iter) {
        MIDI_FreeIterator((midi_track_iter_t *)g_active_midi_iter);
        g_active_midi_iter = NULL;
    }
    g_active_midi_file = NULL;
    ++g_music_generation;
}

boolean I_MusicIsPlaying(void)
{
    return g_music_playing != 0;
}

void I_BindSoundVariables(void)
{
}

void I_SetOPLDriverVer(opl_driver_ver_t ver)
{
}

void I_OPL_DevMessages(char *result, size_t result_len)
{
    if (result && result_len > 0)
    {
        result[0] = '\0';
    }
}
