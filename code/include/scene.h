/**
 * @file scene.h
 * @brief Global rendering orchestration and menu drawing.
 * 
 * This module is responsible for drawing all game elements based on the
 * current state, managing menu backgrounds, and handling cutscene frame loading.
 * 
 * Authors: Andrew Zhuo and Steven Kenneth Darwy
 */

#ifndef SCENE_H
#define SCENE_H

#include "interaction.h"
#include "interactive.h"
#include "settings.h"
#include "map.h"
#include "character.h"
#include "game_context.h"

/**
 * @brief Holds shared textures and timers for non-world rendering (Menus, Cutscenes).
 */
typedef struct Scene {
    Texture2D mainmenu_background;                  // Visual for the start screen
    Texture2D pause_menu_background;                // Dimmed background for pause
    Texture2D vignette;                             // Horror-theming overlay effect
    Texture2D current_cutscene_frame_texture;       // Single frame currently being shown in a cutscene
    Texture2D current_knob_frame_texture;           // Single frame for the volume slider knob
    float cutscene_timer;                           // Progress tracker for cutscene timing
    int current_cutscene_frame;                     // Index of the active cutscene image
} Scene;

/**
 * @brief Loads common menu and overlay textures.
 * @param game_settings Pointer to settings for resolution-specific loading.
 * @return An initialized Scene structure.
 */
Scene InitScene(Settings* game_settings);

/**
 * @brief Top-level drawing dispatcher. 
 * Calls appropriate sub-draw functions based on GaneState.
 */
void DrawGame(
    Scene *game_scene, Settings *game_settings, 
    Interactive *game_interactive, Map *game_map, Character *player,
    Dialogue *game_dialogue, GameContext *game_context,
    GameState *game_state, NPC worldNPCs[], Item worldItems[]
);

/**
 * @brief Loads a specific image from disk into the current_cutscene_frame_texture.
 * This is used for high-frame-count cutscenes to save memory.
 */
void LoadCutsceneFrame(Scene *scene, int frame_index, Settings *game_settings);
void LoadMenuFrame(Scene *scene, int frame_index, bool is_save_available);
void LoadSettingsFrame(Scene *scene, int frame_index);
void LoadKnobFrame(Scene *scene, int frame_index);

/** @brief Draws the main menu UI and background. */
void DrawMainMenu(Scene* scene, Interactive* game_interactive);

/** @brief Draws the pause menu overlay. */
void DrawPauseMenu(Scene* scene, Settings* game_settings, Interactive* game_interactive);

/** @brief Draws the settings configuration interface. */
void DrawSettings(Scene* scene, Settings* game_settings, Interactive* game_interactive);

/**
 * @brief Core world rendering function (Map, Player, NPCs, Items, UI).
 */
void DrawGameplay(
    Scene* scene, Settings* game_settings, Interactive* game_interactive,
    Map* game_map, Character* player, NPC worldNPCs[], Item worldItems[], 
    GameContext* game_context
);

/** @brief Frees textures used during cutscenes. */
void ClearCutscene(Scene* scene);

/** @brief Unloads all scene-managed textures and clean up. */
void CloseScene(Scene* scene);

#endif