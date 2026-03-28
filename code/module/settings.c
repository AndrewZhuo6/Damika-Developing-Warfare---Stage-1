/**
 * @file settings.c
 * @brief Implementation of global game configuration and initialization.
 * 
 * Defines the default startup parameters for the window, audio, and gameplay.
 * 
 * Authors: Andrew Zhuo
 */

#include "settings.h"

Settings InitSettings(){
    // Initialize game settings with hardcoded defaults
    Settings new_settings = {0};
    new_settings.window_width = 1200;
    new_settings.window_height = 800;
    new_settings.fps = 60;
    new_settings.game_volume = 50.0f;
    new_settings.mc_speed = 150.0f;
    new_settings.max_stamina = 100.0f;
    new_settings.stamina_depletion_rate = 20.0f;
    new_settings.stamina_recovery_rate = 10.0f;
    new_settings.hallucination_increase_rate = 0.05f;
    
    return new_settings;
}
