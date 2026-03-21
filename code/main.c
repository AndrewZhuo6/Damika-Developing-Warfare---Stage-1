/**
 * @file main.c
 * @brief Entry point and main game loop for the Aisling game.
 * 
 * This file handles the initialization of all game systems (audio, character, map, etc.),
 * contains the main game loop that updates and draws the game state, and manages
 * the cleanup process when the game exits.
 * 
 * Authors: Andrew Zhuo, Cornelius Jabez Lim, Steven Kenneth Darwy
 */

#include "audio.h"
#include "character.h"
#include "dialogue.h"
#include "game_context.h"
#include "interaction.h"
#include "map.h"
#include "raylib.h"
#include "scene.h"
#include "settings.h"
#include "state.h"
#include "phone.h"
#include <stdio.h>
#include "data.h"

/**
 * @brief Prepares the Raylib window and basic game configuration.
 * @param game_settings Pointer to the game settings structure.
 */
void InitGame(Settings *game_settings);

/**
 * @brief The main execution loop of the game.
 * 
 * Handles input processing, state updates, and rendering until the window is closed.
 * 
 * @param player Pointer to the player character.
 * @param game_audio Pointer to the audio system.
 * @param game_settings Pointer to the game settings.
 * @param game_scene Pointer to the scene management.
 * @param game_interactive Pointer to interactive UI elements.
 * @param game_dialogue Pointer to the dialogue system.
 * @param game_map Pointer to the map data.
 * @param worldNPCs Array of NPCs in the game world.
 * @param worldItems Array of items in the game world.
 * @param game_context Pointer to the global game context.
 * @param game_state Pointer to the current game state enumeration.
 */
void RunGame(Character *player, Audio *game_audio, Settings *game_settings,
             Scene *game_scene, Interactive *game_interactive,
             Dialogue *game_dialogue, Map *game_map,
             NPC worldNPCs[], Item worldItems[], GameContext *game_context,
             GameState *game_state);

/**
 * @brief Performs cleanup of all game resources before exit.
 * @param game_audio Audio system to close.
 * @param player Player character to free.
 * @param worldItems Array of world items.
 * @param itemCount Number of items in the world.
 * @param game_scene Scene to close.
 * @param game_interactive Interactive system to close.
 * @param game_map Map to free.
 * @param game_settings Settings to close.
 */
void EndGame(Audio *game_audio, Character *player, Item worldItems[], int itemCount, Scene *game_scene,
             Interactive *game_interactive, Map *game_map,
             Settings *game_settings);

int main(void){
    // 1. Initialize core settings and windows
    Settings game_settings = InitSettings();
    InitGame(&game_settings);

    // 2. Load persistent game data (e.g., save files)
    Data game_data = LoadData(&game_settings);

    // 3. Initialize all game components/modules
    Map game_map = InitMap("../assets/map/MAINMAP.json");
    Character player = InitCharacter(&game_settings, &game_data, &game_map);
    Audio game_audio = InitAudio(&game_settings);
    Scene game_scene = InitScene(&game_settings);
    Interactive game_interactive = InitInteractive(&game_settings);
    Dialogue game_dialogue = LoadDialogue("../assets/text/dialogue1.txt");
    
    // Register world entities (NPCs and Items)
    NPC worldNPCs[2] = {
        {{{0}, "../assets/images/character/furina.png", {800, 600, 200, 200}, false, INTERACTABLE_TYPE_NPC}, "../assets/text/signpost.txt"},
        {{{0}, "../assets/images/character/oldman.png", {600, 300, 150, 150}, false, INTERACTABLE_TYPE_NPC}, "../assets/text/oldman.txt"},
    };
    Item worldItems[1] = {
        {{{0}, "../assets/images/items/potato.png", {500, 500, 50, 50}, false, INTERACTABLE_TYPE_ITEM}, false}
    };
    
    // Final game context and initial state setup
    GameContext game_context = InitGameContext(&game_map, &player, &game_settings);
    GameState game_state = MAINMENU;

    // 4. Load visual resources for world entities
    LoadNPCs(worldNPCs, 2);
    LoadItems(worldItems, 1);

    // 5. Enter the main execution loop
    RunGame(&player, &game_audio, &game_settings, &game_scene, &game_interactive,
            &game_dialogue, &game_map, worldNPCs, worldItems, &game_context, &game_state);
    
    // 6. Perform cleanup and exit correctly
    EndGame(&game_audio, &player, worldItems, 1, &game_scene, &game_interactive, &game_map, &game_settings);
    
    return 0;
}

void InitGame(Settings *game_settings){
    // Set configuration flags for the Raylib window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(24);
    
    // Create the game window with initial dimensions
    InitWindow(game_settings->window_width, game_settings->window_height, "Aisling");
        
    // Set the application icon
    Image icon = LoadImage("../assets/images/icon/app_icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    
    // Disable default ESC behavior to handle custom pause/exit logic
    SetExitKey(0);
}

void RunGame(Character *player, Audio *game_audio, Settings *game_settings,
        Scene *game_scene, Interactive *game_interactive,
        Dialogue *game_dialogue, Map *game_map,
        NPC worldNPCs[], Item worldItems[], GameContext *game_context,
        GameState *game_state){
    
    Dialogue *current_dialogue = game_dialogue;
    Interactable *objectToInteractWith = NULL;

    // Main Game Loop: continues until window close or quit buttons are pressed
    while (!WindowShouldClose()){
        // Update background music streams
        UpdateAudio(game_audio);

        // Interaction system: calculate proximity-based hitbox
        Rectangle playerHitbox = {player->position.x + 50, player->position.y, 300, 300};
        
        Vector2 map_size = {(float)game_map->tiled_map->width * game_map->tiled_map->tilewidth,
                            (float)game_map->tiled_map->height * game_map->tiled_map->tileheight};

        // Identify objects near the player that can be interacted with
        CheckInteractable(worldNPCs, worldItems, 2, 1, playerHitbox, &objectToInteractWith);

        // Input Handling: Pause Menu (ESC)
        if (IsKeyPressed(KEY_ESCAPE)){
            if (*game_state == PAUSE){
                // Resume game from pause
                if (game_context->previous_state == PHOTO_CUTSCENE) {
                    PauseMusicStream(game_audio->bg_music);
                    ResumeMusicStream(game_audio->cutscene_music);
                }
                *game_state = game_context->previous_state;
            } else if (*game_state == GAMEPLAY || *game_state == PHOTO_CUTSCENE){
                // Switch to pause menu
                if (*game_state == PHOTO_CUTSCENE) {
                    PauseMusicStream(game_audio->cutscene_music);
                    ResumeMusicStream(game_audio->bg_music);
                }
                game_context->previous_state = *game_state;
                *game_state = PAUSE;
                UpdateInteractiveLayout(game_interactive, PAUSE, game_settings);
            }
        }

        // Input Handling: Interaction Trigger (E)
        if (IsKeyPressed(KEY_E) && *game_state == GAMEPLAY) {
            InteractWithObject(objectToInteractWith, current_dialogue, game_state, player);
        }

        // Input Handling: Dialogue Progression (SPACE)
        if (IsKeyPressed(KEY_SPACE) && *game_state == DIALOGUE_CUTSCENE) {
            InteractWithObject(objectToInteractWith, current_dialogue, game_state, player);
        }

        // Update sub-systems (Phone messaging)
        UpdatePhone(&game_context->phone, GetFrameTime());

        // Core logic update: movement, collisions, and state transitions
        if (UpdateGame(
            game_state, game_interactive, player, worldItems, 1, game_settings, game_map,
            game_context, game_audio, map_size, game_scene
        )){
            // Exit loop if UpdateGame returns 1 (Quit clicked)
            break;
        }

        // Responsive UI: update layout on window resize
        if (IsWindowResized()){
            UpdateInteractiveLayout(game_interactive, *game_state, game_settings);
        }

        // Rendering phase: draw current scene based on state
        DrawGame(game_scene, game_settings, game_interactive, game_map, player,
                current_dialogue, game_context, game_state, worldNPCs, worldItems);
    }
}

void EndGame(Audio *game_audio, Character *player, Item worldItems[], int itemCount, Scene *game_scene,
            Interactive *game_interactive, Map *game_map, Settings *game_settings){
    // Save player progress and world state
    SaveData(player, worldItems, itemCount, game_settings);

    // Free all allocated resources to prevent memory leaks
    CloseAudio(game_audio);
    CloseCharacter(player);
    CloseScene(game_scene);
    CloseInteractive(game_interactive);
    FreeMap(game_map);

    // Shutdown Raylib environment
    CloseWindow();
}