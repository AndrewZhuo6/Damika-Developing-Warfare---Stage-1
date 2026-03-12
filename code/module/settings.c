/*
This module stores the implementation of the variable and functions
contained in the settings.h header file.
*/

#include "settings.h"

int window_width = 1200;
int window_height = 800;
int fps = 60;
float game_volume = 50;
float mc_speed = 5.0f;
Vector2 mc_size = {100.0f, 100.0f};

int change_volume(int volume) {
    volume += 5;
    return volume;
}