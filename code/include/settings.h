/*
This file contains variable declarations and function prototypes for the
settings module.

Module made by Andrew Zhuo.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "raylib.h"

typedef struct Settings{
    /* This struct contains the information for the settings in the game. */
    int window_width;                   // Width of the game window.
    int window_height;                  // Height of the game window.
    int fps;                            // Frames per second.
    float game_volume;                  // Volume of the game.
    float mc_speed;                     // Speed of the main character.
    float max_stamina;                  // Maximum stamina of the character.
    float stamina_depletion_rate;       // Rate at which stamina decreases (per second).
    float stamina_recovery_rate;        // Rate at which stamina increases (per second).
} Settings;

Settings InitSettings();     // Initializes the game settings.

#endif