/**
 * @file scene.c
 * @brief Implementation of high-level rendering and scene management.
 * 
 * Orchestrates the drawing of backgrounds, menus, gameplay, and post-processing
 * effects like hallucination darkness and vignette.
 * 
 * Authors: Andrew Zhuo, Cornelius Jabez Lim, and Steven Kenneth Darwy
 */

#include "scene.h"
#include <stdio.h>

/** @brief Loads core static backgrounds and initializes cutscene state. */
Scene InitScene(Settings* game_settings){
    Scene new_scene = {0};

    // Load static backgrounds
    new_scene.mainmenu_background = LoadTexture("../assets/images/background/main_menu/main_menu.png");
    new_scene.pause_menu_background = LoadTexture("../assets/images/background/pause/pause.png");
    new_scene.vignette = LoadTexture("../assets/images/background/vignette/vignette.png");
    
    // Cutscene defaults
    new_scene.current_cutscene_frame_texture = (Texture2D){0};
    new_scene.current_knob_frame_texture = (Texture2D){0};
    new_scene.cutscene_timer = 0.0f;
    new_scene.current_cutscene_frame = 0;

    return new_scene;
}

/**
 * @brief Top-level rendering entry point.
 * 
 * Dispatches to state-specific drawing functions and applies global 
 * overlays (Vignette, Hallucination Darkness, Dialogue UI).
 */
void DrawGame(Scene *game_scene, Settings *game_settings, 
              Interactive *game_interactive, Map *game_map, Character *player,
              Dialogue *game_dialogue, GameContext *game_context,
              GameState *game_state, NPC worldNPCs[], Item worldItems[]){
    
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // 1. State Dispatch
    if (*game_state == MAINMENU) {
        DrawMainMenu(game_scene, game_interactive);
    } else if (*game_state == SETTINGS) {
        DrawSettings(game_scene, game_settings, game_interactive);
    } else if (*game_state == PAUSE) {
        DrawPauseMenu(game_scene, game_settings, game_interactive);
    } else if (*game_state == PHOTO_CUTSCENE) {
        // Full-screen frame-by-frame cutscene playback
        DrawTexturePro(game_scene->current_cutscene_frame_texture,
            (Rectangle){0, 0, (float)game_scene->current_cutscene_frame_texture.width, (float)game_scene->current_cutscene_frame_texture.height},
            (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            (Vector2){0, 0}, 0.0f, WHITE);
            
        if (game_scene->cutscene_timer < 5.0f) {
            DrawText("Double click 'Enter' to skip", GetScreenWidth() - 300, GetScreenHeight() - 40, 20, LIGHTGRAY);
        }
    } else {
        // 2. Active Gameplay Layer
        DrawGameplay(game_scene, game_settings, game_interactive, game_map, player, worldNPCs, worldItems, game_context);
        
        // --- Post-Processing Layer ---
        
        // Permanent Vignette (Claustrophobia effect)
        DrawTexturePro(game_scene->vignette,
            (Rectangle){0, 0, (float)game_scene->vignette.width, (float)game_scene->vignette.height},
            (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            (Vector2){0, 0}, 0.0f, WHITE);
        
        // Hallucination 'Blackout' Darkness
        // Triggers once the bar is full; scales alpha until complete darkness
        float darkness_alpha = (player->hallucination - player->max_hallucination) * 5.0f / player->max_hallucination;
        if (darkness_alpha > 0.0f){
            if (darkness_alpha > 1.0f) darkness_alpha = 1.0f;
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, darkness_alpha));
        }

        // --- UI / Overlay Layer ---
        if (*game_state == DIALOGUE_CUTSCENE){
            DrawRectangle(0, GetScreenHeight() - 120, GetScreenWidth(), 120, Fade(BLACK, 0.8f));

            const char *line = game_dialogue->lines[game_dialogue->current_line];
            DrawText(line, GetScreenWidth() / 2 - MeasureText(line, 20) / 2,
                    GetScreenHeight() - 80, 20, WHITE);
            DrawText("Press 'SPACE' to continue", GetScreenWidth() - 300,
                    GetScreenHeight() - 40, 20, GRAY);
        }

        DrawPhone(&game_context->phone);
    }
    EndDrawing();
}

/** @brief Renders the Main Menu background and interactive buttons. */
void DrawMainMenu(Scene* scene, Interactive* game_interactive){
    DrawTexturePro(scene->current_cutscene_frame_texture, 
        (Rectangle){0, 0, (float)scene->current_cutscene_frame_texture.width, (float)scene->current_cutscene_frame_texture.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);
    
    if (game_interactive->is_continue_hovered) DrawRectangleRec(game_interactive->continue_bounds, Fade(WHITE, 0.3f));
    if (game_interactive->is_new_game_hovered) DrawRectangleRec(game_interactive->new_game_bounds, Fade(WHITE, 0.3f));
    if (game_interactive->is_settings_hovered) DrawRectangleRec(game_interactive->settings_bounds, Fade(WHITE, 0.3f));
    if (game_interactive->is_quit_hovered) DrawRectangleRec(game_interactive->quit_bounds, Fade(WHITE, 0.3f));
}

/** @brief Renders the Pause Menu background and its specific button subset. */
void DrawPauseMenu(Scene* scene, Settings* game_settings, Interactive* game_interactive){
    DrawTexturePro(scene->pause_menu_background, 
        (Rectangle){0, 0, (float)scene->pause_menu_background.width, (float)scene->pause_menu_background.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);
    
    DrawTexture(game_interactive->continue_button, game_interactive->continue_bounds.x, game_interactive->continue_bounds.y, game_interactive->is_continue_hovered ? GRAY : WHITE);
    DrawTexture(game_interactive->settings_button, game_interactive->settings_bounds.x, game_interactive->settings_bounds.y, game_interactive->is_settings_hovered ? GRAY : WHITE);
    DrawTexture(game_interactive->main_menu_button, game_interactive->main_menu_bounds.x, game_interactive->main_menu_bounds.y, game_interactive->is_main_menu_hovered ? GRAY : WHITE);
    DrawTexture(game_interactive->quit_button, game_interactive->quit_bounds.x, game_interactive->quit_bounds.y, game_interactive->is_quit_hovered ? GRAY : WHITE);
}

/** @brief Renders the Settings screen with volume slider. */
void DrawSettings(Scene* scene, Settings* game_settings, Interactive* game_interactive){
    DrawTexturePro(scene->current_cutscene_frame_texture, 
        (Rectangle){0, 0, (float)scene->current_cutscene_frame_texture.width, (float)scene->current_cutscene_frame_texture.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);

    // --- Draw Animated Knob ---
    DrawTexturePro(scene->current_knob_frame_texture,
        (Rectangle){0, 0, (float)scene->current_knob_frame_texture.width, (float)scene->current_knob_frame_texture.height},
        game_interactive->volume_slider_knob,
        (Vector2){0, 0}, 0.0f, WHITE);

    if (game_interactive->is_settings_back_hovered) DrawRectangleRec(game_interactive->settings_back_bounds, Fade(WHITE, 0.3f));
}

/** 
 * @brief Logic for rendering the 2D world inside the camera's perspective.
 * 
 * Maps, characters, NPCs, and world-items are drawn here. 
 */
void DrawGameplay(Scene* scene, Settings* game_settings, Interactive* game_interactive, Map* game_map,
                Character* player, NPC worldNPCs[], Item worldItems[], GameContext* game_context){
    BeginMode2D(game_context->camera);
    DrawMap(game_map);
    DrawCharacter(player);
    
    // Draw World NPCs
    for (int i = 0; i < 2; i++){
        DrawTexturePro(worldNPCs[i].base.texture,
            (Rectangle){0, 0, (float)worldNPCs[i].base.texture.width, (float)worldNPCs[i].base.texture.height},
            worldNPCs[i].base.bounds, (Vector2){0, 0}, 0, WHITE);
            
        // Indicator if in range
        if (worldNPCs[i].base.isActive){
            DrawText("!", worldNPCs[i].base.bounds.x + worldNPCs[i].base.bounds.width / 2, worldNPCs[i].base.bounds.y - 40, 50, RED);
        }
    }
    
    // Draw World Items
    for (int i = 0; i < 1; i++){
        if (!worldItems[i].picked_up){
            DrawTexturePro(worldItems[i].base.texture,
                (Rectangle){0, 0, (float)worldItems[i].base.texture.width, (float)worldItems[i].base.texture.height},
                worldItems[i].base.bounds, (Vector2){0, 0}, 0, WHITE);
            if (worldItems[i].base.isActive){
                DrawText("!", worldItems[i].base.bounds.x + 20, worldItems[i].base.bounds.y - 30, 50, RED);
            }
        }
    }
    EndMode2D();
}

/**
 * @brief Loads a single frame for the intro cutscene on-demand.
 * 
 * To save VRAM, each frame (QOI format) is loaded, drawn, and unloaded immediately.
 */
void LoadCutsceneFrame(Scene *scene, int frame_index, Settings *game_settings){
    // Clean up previous frame
    if (scene->current_cutscene_frame_texture.id != 0){
        UnloadTexture(scene->current_cutscene_frame_texture);
    }
    
    char path[100];
    sprintf(path, "../assets/videos/cutscene/frame%04d.qoi", frame_index);
    scene->current_cutscene_frame_texture = LoadTexture(path);
}

void LoadMenuFrame(Scene *scene, int frame_index, bool is_save_available) {
    if (scene->current_cutscene_frame_texture.id != 0){
        UnloadTexture(scene->current_cutscene_frame_texture);
    }
    
    char path[100];
    const char* folder = is_save_available ? "saved_game" : "new_game";
    sprintf(path, "../assets/videos/main_menu/%s/frame%04d.qoi", folder, frame_index);
    scene->current_cutscene_frame_texture = LoadTexture(path);
}

void LoadSettingsFrame(Scene *scene, int frame_index) {
    if (scene->current_cutscene_frame_texture.id != 0){
        UnloadTexture(scene->current_cutscene_frame_texture);
    }
    
    char path[100];
    sprintf(path, "../assets/videos/slider_bar/frame%04d.qoi", frame_index);
    scene->current_cutscene_frame_texture = LoadTexture(path);
}

void LoadKnobFrame(Scene *scene, int frame_index) {
    if (scene->current_knob_frame_texture.id != 0){
        UnloadTexture(scene->current_knob_frame_texture);
    }
    
    char path[100];
    sprintf(path, "../assets/videos/slider_knob/frame%04d.qoi", frame_index);
    scene->current_knob_frame_texture = LoadTexture(path);
}

/** @brief Safely unloads the intro cutscene frame buffer. */
void ClearCutscene(Scene* scene){
    if (scene->current_cutscene_frame_texture.id != 0){
        UnloadTexture(scene->current_cutscene_frame_texture);
        scene->current_cutscene_frame_texture.id = 0;
    }
}

/** @brief Frees all persistent backgrounds and memory allocated by the scene. */
void CloseScene(Scene* scene){
    UnloadTexture(scene->mainmenu_background);
    UnloadTexture(scene->pause_menu_background);
    UnloadTexture(scene->vignette);
    
    if (scene->current_knob_frame_texture.id != 0){
        UnloadTexture(scene->current_knob_frame_texture);
    }
    
    ClearCutscene(scene);
}
