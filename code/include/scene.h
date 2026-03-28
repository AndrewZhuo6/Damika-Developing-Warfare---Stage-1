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
 *
 * @param game_settings Pointer to settings for resolution-specific loading.
 * @return An initialized Scene structure.
 */
Scene InitScene(Settings* game_settings);

/**
 * @brief Top-level drawing dispatcher. 
 * Calls appropriate sub-draw functions based on GaneState.
 *
 * @param game_scene Pointer to the scene.
 * @param game_settings Pointer to the settings.
 * @param game_interactive Pointer to the interactive.
 * @param game_map Pointer to the map.
 * @param player Pointer to the player.
 * @param game_dialogue Pointer to the dialogue.
 * @param game_context Pointer to the game context.
 * @param game_state Pointer to the game state.
 * @param worldNPCs Array of NPCs.
 * @param worldItems Array of items.
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
 *
 * @param scene Pointer to the scene.
 * @param frame_index Index of the frame to load.
 * @param game_settings Pointer to the settings.
 */
void LoadCutsceneFrame(Scene *scene, int frame_index, Settings *game_settings);

/**
 * @brief Loads a specific image from disk into the current_menu_frame_texture.
 *
 * @param scene Pointer to the scene.
 * @param frame_index Index of the frame to load.
 * @param is_save_available Whether a save is available.
 */
void LoadMenuFrame(Scene *scene, int frame_index, bool is_save_available);

/**
 * @brief Loads a specific image from disk into the current_settings_frame_texture.
 *
 * @param scene Pointer to the scene.
 * @param frame_index Index of the frame to load.
 */
void LoadSettingsFrame(Scene *scene, int frame_index);

/**
 * @brief Loads a specific image from disk into the current_knob_frame_texture.
 *
 * @param scene Pointer to the scene.
 * @param frame_index Index of the frame to load.
 */
void LoadKnobFrame(Scene *scene, int frame_index);

/**
 * @brief Draws the main menu UI and background.
 *
 * @param scene Pointer to the scene.
 * @param game_interactive Pointer to the interactive.
 */
void DrawMainMenu(Scene* scene, Interactive* game_interactive);

/**
 * @brief Draws the pause menu overlay.
 *
 * @param scene Pointer to the scene.
 * @param game_settings Pointer to the settings.
 * @param game_interactive Pointer to the interactive.
 */
void DrawPauseMenu(Scene* scene, Settings* game_settings, Interactive* game_interactive);

/**
 * @brief Draws the settings configuration interface.
 *
 * @param scene Pointer to the scene.
 * @param game_settings Pointer to the settings.
 * @param game_interactive Pointer to the interactive.
 */
void DrawSettings(Scene* scene, Settings* game_settings, Interactive* game_interactive);

/**
 * @brief Core world rendering function (Map, Player, NPCs, Items, UI).
 *
 * @param scene Pointer to the scene.
 * @param game_settings Pointer to the settings.
 * @param game_interactive Pointer to the interactive.
 * @param game_map Pointer to the map.
 * @param player Pointer to the player.
 * @param worldNPCs Array of NPCs.
 * @param worldItems Array of items.
 * @param game_context Pointer to the game context.
 */
void DrawGameplay(
    Scene* scene, Settings* game_settings, Interactive* game_interactive,
    Map* game_map, Character* player, NPC worldNPCs[], Item worldItems[], 
    GameContext* game_context
);

/**
 * @brief Frees textures used during cutscenes.
 *
 * @param scene Pointer to the scene.
 */
void ClearCutscene(Scene* scene);

/**
 * @brief Unloads all scene-managed textures and clean up.
 *
 * @param scene Pointer to the scene.
 */
void CloseScene(Scene* scene);

#endif