/*
This file contains function prototypes for the audio module.

Module made by Andrew Zhuo.
*/

#ifndef AUDIO_H
#define AUDIO_H

#include "settings.h"

typedef struct Audio{
    /* This struct contains the information for audio in the game. */
    Music bg_music;             // Background music
    Sound scream_sound;         // Scream sound
    Sound step_outdoor;         // Outdoor walking sound
    Sound step_indoor;          // Indoor walking sound
    Sound notif_sound;          // Notification sound
} Audio;

Audio InitAudio(Settings* game_settings);              // Initializes the game's audio.
void UpdateAudio(Audio* audio);                        // Keep updating background music buffer.
void CloseAudio(Audio* audio);                         // Close the audio device.
void PlayScream(Audio* audio);                         // Play the scream sound.
void PlayStep(Audio* audio, bool is_outdoor);          // Play the walking sound.
void PlayNotif(Audio* audio);                          // Play the notification sound.

#endif
