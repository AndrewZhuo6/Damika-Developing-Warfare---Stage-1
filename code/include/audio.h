/*
This file contains function prototypes for the audio module.
*/

#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

typedef struct audio{
    /* This struct contains the audios that the game has. */
    Music bg_music;                 // Background music
    Sound scream_sound;             // Scream sound
} audio;

audio audio_init();                 // Initializes the game's audio.
void audio_update(audio* audio);    // Keep updating background music buffer.
void audio_close(audio* audio);     // Close the audio device.
void play_scream(audio* audio);     // Play the scream sound.

#endif
