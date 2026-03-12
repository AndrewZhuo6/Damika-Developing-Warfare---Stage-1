/*
This module stores the implementation of the variable and functions
contained in the mc.h header file.
*/

#include "settings.h"
#include "mc.h"

mc mc_init(){
    mc new_mc = {0};
    new_mc.position = (Vector2){(float)window_width / 2, (float)window_height / 2};
    new_mc.size = mc_size;
    new_mc.x_direction = 1;
    new_mc.y_direction = 1;

    Image img;
    img = LoadImage("../assets/images/mc/mc_top_right.png");
    ImageResize(&img, (int)mc_size.x, (int)mc_size.y);
    new_mc.t_right = LoadTextureFromImage(img);
    UnloadImage(img);

    img = LoadImage("../assets/images/mc/mc_top_left.png");
    ImageResize(&img, (int)mc_size.x, (int)mc_size.y);
    new_mc.t_left = LoadTextureFromImage(img);
    UnloadImage(img);

    img = LoadImage("../assets/images/mc/mc_bottom_right.png");
    ImageResize(&img, (int)mc_size.x, (int)mc_size.y);
    new_mc.b_right = LoadTextureFromImage(img);
    UnloadImage(img);

    img = LoadImage("../assets/images/mc/mc_bottom_left.png");
    ImageResize(&img, (int)mc_size.x, (int)mc_size.y);
    new_mc.b_left = LoadTextureFromImage(img);
    UnloadImage(img);

    new_mc.character = new_mc.b_right;
    
    return new_mc;
}

void mc_update(mc* mc) {
    if (mc->moving_right){
        mc->position.x += mc_speed;
        if (mc->y_direction){
            mc->character = mc->b_right;
        } else{
            mc->character = mc->t_right;
        }
        mc->x_direction = 1;
    }
    if (mc->moving_left){
        mc->position.x -= mc_speed;
        if (mc->y_direction){
            mc->character = mc->b_left;
        } else{
            mc->character = mc->t_left;
        }
        mc->x_direction = 0;
    }
    if (mc->moving_up){
        mc->position.y -= mc_speed;
        if (mc->x_direction){
            mc->character = mc->t_right;
        } else{
            mc->character = mc->t_left;
        }
        mc->y_direction = 0;
    }
    if (mc->moving_down){
        mc->position.y += mc_speed;
        if (mc->x_direction){
            mc->character = mc->b_right;
        } else{
            mc->character = mc->b_left;
        }
        mc->y_direction = 1;
    }
}