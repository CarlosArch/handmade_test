#include <DSound.h>
#include "my_definitions.h"

/* object orientation in C is shit */

LPDIRECTSOUNDBUFFER initialize_directsound(
        HWND window_handle,
        int32 number_of_channels,
        int32 samples_per_second,
        int32 bits_per_sample,
        int32 buffer_size
        )
{
    LPDIRECTSOUND direct_sound_pointer;
    { // Initialize directsound
        DirectSoundCreate(0, &direct_sound_pointer, 0);

        IDirectSound_SetCooperativeLevel(
            direct_sound_pointer,
            window_handle,
            DSSCL_PRIORITY
        );
    }
    WAVEFORMATEX wave_format = {
        /* wFormatTag */      WAVE_FORMAT_PCM,
        /* nChannels */       number_of_channels,
        /* nSamplesPerSec */  samples_per_second,
        /* nAvgBytesPerSec */ samples_per_second * (number_of_channels * bits_per_sample) / 8,
        /* nBlockAlign */     (number_of_channels * bits_per_sample) / 8,
        /* wBitsPerSample */  bits_per_sample,
        /* cbSize */          0
    };
    LPDIRECTSOUNDBUFFER primary_buffer_pointer;
    {// Initialize primary buffer
        DSBUFFERDESC primary_buffer_description = {
            /* dwSize */          sizeof(DSBUFFERDESC),
            /* dwFlags */         DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER,
            /* dwBufferBytes */   0,
            /* dwReserved */      0,
            /* lpwfxFormat */     0,
            /* guid3DAlgorithm */ DS3DALG_DEFAULT
        };
        IDirectSound_CreateSoundBuffer(
            direct_sound_pointer,
            &primary_buffer_description,
            &primary_buffer_pointer,
            0
        );

        IDirectSoundBuffer_SetFormat(primary_buffer_pointer, &wave_format);
    }
    LPDIRECTSOUNDBUFFER secondary_buffer_pointer;
    {// Initialize secondary buffer
        DSBUFFERDESC secondary_buffer_description = {
            /* dwSize */          sizeof(DSBUFFERDESC),
            /* dwFlags */         DSBCAPS_CTRLVOLUME,
            /* dwBufferBytes */   buffer_size,
            /* dwReserved */      0,
            /* lpwfxFormat */     &wave_format,
            /* guid3DAlgorithm */ DS3DALG_DEFAULT
        };
        IDirectSound_CreateSoundBuffer(
            direct_sound_pointer,
            &secondary_buffer_description,
            &secondary_buffer_pointer,
            0
        );
    }
    return secondary_buffer_pointer;
}