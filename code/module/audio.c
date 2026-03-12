/*
This module stores the implementation of the functions contained in the
audio.h header file.
*/

#include "settings.h"
#include "audio.h"

audio audio_init(){
    /* Initialize audio device and loading game audio. */
    audio new_audio = {0};
    InitAudioDevice();
    SetMasterVolume(game_volume);
    new_audio.bg_music = LoadMusicStream("../assets/audios/bg_music.ogg");
    new_audio.scream_sound = LoadSound("../assets/audios/ghost_scream.wav");

    // Play background music.
    PlayMusicStream(new_audio.bg_music);

    // return the game audio.
    return new_audio;
}

void audio_update(audio* audio){
    /* Keep updating the background music buffer. */
    UpdateMusicStream(audio->bg_music);
}

void audio_close(audio* audio){
    /* Unload audio and close audio device. */
    UnloadMusicStream(audio->bg_music);
    CloseAudioDevice();
}

void play_scream(audio* audio){
    /* Play scream sound. */
    PlaySound(audio->scream_sound);
}