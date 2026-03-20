/*
This module stores the implementation of the functions contained in the
audio.h header file.

Module made by Andrew Zhuo.
*/

#include "audio.h"

Audio InitAudio(Settings* game_settings){
    /* Initialize audio device and loading game audio. */

    // Initialize audio device
    Audio new_audio = {0};
    InitAudioDevice();

    // Set master volume
    SetMasterVolume(game_settings->game_volume);

    // Load game audio
    new_audio.bg_music = LoadMusicStream("../assets/audios/bg_music.ogg");
    new_audio.scream_sound = LoadSound("../assets/audios/ghost_scream.wav");
    new_audio.step_outdoor = LoadSound("../assets/audios/step_outdoor.mp3");
    new_audio.step_indoor = LoadSound("../assets/audios/step_indoor.mp3");
    new_audio.notif_sound = LoadSound("../assets/audios/notif.wav");
    new_audio.cutscene_music = LoadMusicStream("../assets/audios/cutscene.mp3");

    // Play background music
    PlayMusicStream(new_audio.bg_music);

    return new_audio;
}

void UpdateAudio(Audio* audio){
    /* Keep updating the music buffers. */
    UpdateMusicStream(audio->bg_music);
    UpdateMusicStream(audio->cutscene_music);
}

void CloseAudio(Audio* audio){
    /* Unload audio and close audio device. */
    UnloadMusicStream(audio->bg_music);
    UnloadMusicStream(audio->cutscene_music);
    UnloadSound(audio->scream_sound);
    UnloadSound(audio->step_outdoor);
    UnloadSound(audio->step_indoor);
    UnloadSound(audio->notif_sound);
    CloseAudioDevice();
}

void PlayScream(Audio* audio){
    /* Play scream sound. */
    if (!IsSoundPlaying(audio->scream_sound)){
        PlaySound(audio->scream_sound);
    }
}

void PlayNotif(Audio* audio){
    /* Play notification sound. */
    if (!IsSoundPlaying(audio->notif_sound)){
        PlaySound(audio->notif_sound);
    }
}

void PlayStep(Audio* audio, bool is_outdoor){
    /* Play walking sound if it's not already playing. */
    if (!IsSoundPlaying(audio->step_outdoor) && is_outdoor){
        PlaySound(audio->step_outdoor);
    } else if (!IsSoundPlaying(audio->step_indoor) && !is_outdoor){
        PlaySound(audio->step_indoor);
    }
}