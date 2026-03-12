/*
This file contains variable declarations and function prototypes for the
mc module.
*/

#ifndef MC_H
#define MC_H

#include <stdbool.h>
#include "raylib.h"

typedef struct mc{
    Vector2 position;
    Texture2D character;
    Texture2D t_right;
    Texture2D t_left;
    Texture2D b_right;
    Texture2D b_left;
    Vector2 size;
    int x_direction;
    int y_direction;

    // Movement flags.
    bool moving_right;
    bool moving_left;
    bool moving_up;
    bool moving_down;
} mc;

mc mc_init();
void mc_update(mc* mc);

#endif