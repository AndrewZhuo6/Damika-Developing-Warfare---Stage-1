/**
 * @file data.c
 * @brief Implementation of the game's persistence (Save/Load) system.
 * 
 * Uses binary file I/O to store and retrieve the player's state,
 * inventory, world progress, and user settings.
 * 
 * Authors: Andrew Zhuo
 */

#include <string.h>
#include "data.h"
#include "raylib.h"
#include "settings.h"
#include "game_context.h"
#include "assets.h"
#include "story.h"
#include <stdio.h>

/**
 * @brief Loads the Data struct from the filesystem.
 */
Data LoadData(Settings* game_settings){
    Data data = {0};
    int file_size = 0;

    // Check if the data file exists
    if (FileExists("../data/data.dat")){
        unsigned char* file_data = LoadFileData("../data/data.dat", &file_size);
        // Check if the data file is valid
        if (file_data != NULL && file_size == sizeof(Data)){
            memcpy(&data, file_data, sizeof(Data));       // Copy the data from the file to the data struct
        }
        UnloadFileData(file_data);                      // Unload the data from the file
    } else {
        data.position = (Vector2){-1.0f, -1.0f};        // Set the position to -1, -1 to indicate that the data has not been loaded
        data.volume = game_settings->game_volume;       // Set the volume to the game settings
    }
    return data;
}

/**
 * @brief Maps values from a Data struct onto active game objects.
 */
void ApplyData(struct GameContext* context, Settings* game_settings, Data* data){
    Character* player = context->player;
    
    // 1. Restore Player State
    if (data->position.x != -1.0f) player->position = data->position;
    player->direction = data->direction;
    player->sanity = data->sanity;
    for (int i = 0; i < data->inventory_count; i++){
        strcpy(player->inventory[i], data->inventory[i]);
        player->item_count[i] = data->item_count[i];
    }
    player->inventory_count = data->inventory_count;

    // 2. Restore Story State
    if (strlen(data->day_folder) > 0) {
        char path[128];
        sprintf(path, "../assets/text/%s/%s.txt", data->day_folder, data->day_folder);
        LoadStoryDay(&context->story, path);
        context->story.current_set_idx = data->set_idx;
        context->story.current_phase_idx = data->phase_idx;
        context->location = (Location)data->location;
        
        // Restore quest completion for active phase
        StoryPhase* active = GetActivePhase(&context->story);
        if (active) {
            for (int i = 0; i < active->quest_count && i < 10; i++) {
                active->quests[i].completed = data->quest_completion[i];
            }
        }
    }

    // 3. Restore World State
    for (int i = 0; i < context->itemCount && i < 100; i++){
        context->worldItems[i].picked_up = data->picked_up_items[i];
    }

    // 4. Restore Karma
    SetRegistryKarma(data->npc_karma, 64);

    // 5. Restore Settings
    game_settings->game_volume = data->volume;
    SetMasterVolume(game_settings->game_volume / 100.0f);
}

/**
 * @brief Serializes current game state and writes it to a binary file.
 */
void SaveData(struct GameContext* context, Settings* game_settings){
    Data data = {0};
    Character* player = context->player;
    StorySystem* story = &context->story;

    // 1. Harvest Player State
    data.position = player->position;
    data.direction = player->direction;
    data.sanity = player->sanity;
    for (int i = 0; i < player->inventory_count; i++){
        strcpy(data.inventory[i], player->inventory[i]);
        data.item_count[i] = player->item_count[i];
    }
    data.inventory_count = player->inventory_count;

    // 2. Harvest Story State
    strncpy(data.day_folder, story->day_folder, 31);
    data.set_idx = story->current_set_idx;
    data.phase_idx = story->current_phase_idx;
    data.location = (int)context->location;

    StoryPhase* active = GetActivePhase(story);
    if (active) {
        for (int i = 0; i < active->quest_count && i < 10; i++) {
            data.quest_completion[i] = active->quests[i].completed;
        }
    }

    // 3. Harvest World State
    for (int i = 0; i < context->itemCount && i < 100; i++){
        data.picked_up_items[i] = context->worldItems[i].picked_up;
    }

    // 4. Harvest Karma
    GetRegistryKarma(data.npc_karma, 64);

    // 5. Harvest Settings
    if (game_settings) data.volume = game_settings->game_volume;
    else data.volume = 100.0f; // Default if NULL

    // Create the data directory if it doesn't exist
    if (!DirectoryExists("../data")) MakeDirectory("../data");
    // Save the data to the data file
    SaveFileData("../data/data.dat", &data, sizeof(Data));
    TraceLog(LOG_INFO, "GAME AUTO-SAVED: S%d P%d at Loc %d", data.set_idx, data.phase_idx, data.location);
}

/**
 * @brief Hard reset of local object state (New Game logic).
 */
void ResetGameData(struct GameContext* context, Vector2 default_spawn){
    Character* player = context->player;
    
    // Reset Player
    player->position = default_spawn;
    player->inventory_count = 0;
    player->sanity = 0.0f;
    player->direction = 0;
    
    // Reset World
    for (int i = 0; i < context->itemCount; i++){
        context->worldItems[i].picked_up = false;
    }
    
    // Reset Story
    context->story.current_set_idx = 0;
    context->story.current_phase_idx = 0;
    
    // Reset Karma
    int zero_karma[64] = {0};
    SetRegistryKarma(zero_karma, 64);
}

/**
 * @brief High-level orchestrator for data handling during startup.
 */
void HandleGameData(struct GameContext* context, Map* game_map, Settings* game_settings){
    Data data = LoadData(game_settings);
    
    if (data.position.x == -1.0f){
        ResetGameData(context, game_map->spawn_position);
    } else {
        ApplyData(context, game_settings, &data); 
    }
}