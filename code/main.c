/* This file contains the main logic for the game. */

#include "raylib.h"
#include "settings.h"
#include "mc.h"
#include "audio.h"

void init_game();
void run_game(mc* player, audio* game_audio);
void check_movement(mc* player);

int main(void){
    // Initialize the game.
    init_game();

    // Load game resources.
    mc player = mc_init();
    audio game_audio = audio_init();

    // Run the game.
    run_game(&player, &game_audio);
    return 0;
}

void init_game(){
    // Prepare and initialize the game windows.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(fps);
    InitWindow(window_width, window_height, "Top Secret");
}

void run_game(mc* player, audio* game_audio){
    while (!WindowShouldClose()){
        // Update audio stream.
        audio_update(game_audio);

        // Detects player movement.
        check_movement(player);
        if (player->position.x == 200){
            play_scream(game_audio);
        }
        
        // Draw game assets to the screen.
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(player->character, player->position.x, player->position.y, WHITE);
        mc_update(player);
        EndDrawing();
    }

    // Prepare to stop the game.
    audio_close(game_audio);

    // Close the game window.
    CloseWindow();
}

void check_movement(mc* player){
    // Check for player movement.
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
        player->moving_left = true;
    } else{
        player->moving_left = false;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
        player->moving_right = true;
    } else{
        player->moving_right = false;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)){
        player->moving_up = true;
    } else{
        player->moving_up = false;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)){
        player->moving_down = true;
    } else{
        player->moving_down = false;
    }
}