#include "interaction.h"
#include <string.h>
#include <stdio.h>
#include "raymath.h"
#include "assets.h"
#include <stdbool.h>

static char current_interactable_id[64] = "";       // The ID of the current interactable object

void LoadNPCs(NPC npcs[], int count){
    for (int i = 0; i < count; i++){
        npcs[i].base.texture = LoadTexture(npcs[i].base.texturePath);
    }
}

void LoadItems(Item items[], int count){
    for (int i = 0; i < count; i++){
        items[i].base.texture = LoadTexture(items[i].base.texturePath);
    }
}

void UnloadNPCs(NPC npcs[], int count){
    for (int i = 0; i < count; i++){
        UnloadTexture(npcs[i].base.texture);
    }
}

void UnloadItems(Item items[], int count){
    for (int i = 0; i < count; i++){
        UnloadTexture(items[i].base.texture);
    }
}

void CheckInteractable(NPC worldNPCs[], Item worldItems[], Door worldDoors[],  int npcCount,
    int itemCount, int doorCount, Rectangle playerHitbox, Interactable** objectToInteractWith){
    
    *objectToInteractWith = NULL;     // Set the object to interact with to NULL
    float min_dist = 1e6;             // Set the minimum distance to a large value
    Vector2 playerPos = {playerHitbox.x + playerHitbox.width / 2.0f, playerHitbox.y + playerHitbox.height / 2.0f};

    // Check for nearby NPCs
    for (int i = 0; i < npcCount; i++){
        if (CheckCollisionRecs(playerHitbox, worldNPCs[i].base.bounds)){
            worldNPCs[i].base.isActive = true;
            Vector2 npcPos = {worldNPCs[i].base.bounds.x + worldNPCs[i].base.bounds.width / 2.0f, 
                               worldNPCs[i].base.bounds.y + worldNPCs[i].base.bounds.height / 2.0f};
            float dist = Vector2Distance(playerPos, npcPos);
            if (dist < min_dist){
                min_dist = dist;
                *objectToInteractWith = (Interactable*)&worldNPCs[i];
            }
        } else {worldNPCs[i].base.isActive = false;}
    }

    // Check for nearby items
    for (int i = 0; i < itemCount; i++){
        if (!worldItems[i].picked_up && CheckCollisionRecs(playerHitbox, worldItems[i].base.bounds)){
            worldItems[i].base.isActive = true;
            Vector2 itemPos = {worldItems[i].base.bounds.x + worldItems[i].base.bounds.width / 2.0f, 
                                worldItems[i].base.bounds.y + worldItems[i].base.bounds.height / 2.0f};
            float dist = Vector2Distance(playerPos, itemPos);
            if (dist < min_dist){
                min_dist = dist;
                *objectToInteractWith = (Interactable*)&worldItems[i];
            }
        } else {worldItems[i].base.isActive = false;}
    }

    // Check for nearby doors
    for (int i = 0; i < doorCount; i++){
        if (CheckCollisionRecs(playerHitbox, worldDoors[i].base.bounds)){
            worldDoors[i].base.isActive = true;
            Vector2 doorPos = {worldDoors[i].base.bounds.x + worldDoors[i].base.bounds.width / 2.0f, 
                                worldDoors[i].base.bounds.y + worldDoors[i].base.bounds.height / 2.0f};
            float dist = Vector2Distance(playerPos, doorPos);
            if (dist < min_dist){
                min_dist = dist;
                *objectToInteractWith = (Interactable*)&worldDoors[i];
            }
        } else {worldDoors[i].base.isActive = false;}
    }
}

void InteractWithObject(Interactable* objectToInteractWith, Dialogue* game_dialogue, 
    GameState* game_state, Character *player, Map *map, struct GameContext *game_context){
    
    // If the game state is dialogue cutscene, interact with the NPC
    if (*game_state == DIALOGUE_CUTSCENE){
        InteractWithNPC(NULL, game_dialogue, game_state, game_context);
        return;
    }

    // If the object to interact with is NULL, return
    if (objectToInteractWith == NULL){
        return;
    }

    // Switch the type of the object to interact with
    switch (objectToInteractWith->type){
        case INTERACTABLE_TYPE_NPC:
            InteractWithNPC((NPC*)objectToInteractWith, game_dialogue, game_state, game_context);
            break;
        case INTERACTABLE_TYPE_ITEM:
            InteractWithItem((Item*)objectToInteractWith, game_dialogue, game_state, player, game_context);
            break;
        case INTERACTABLE_TYPE_DOOR:
            InteractWithDoor((Door*)objectToInteractWith, map, player, game_context);
            break;
    }

    // Story Advance Check
    StoryPhase* active = GetActivePhase(&game_context->story);
    if (active && strcmp(active->name, "SET1-PHASE1") == 0){
        if (active->quest_count > 1) active->quests[1].completed = true;
    }
}

void InteractWithNPC(NPC *npc, Dialogue *game_dialogue, GameState *game_state, struct GameContext *game_context){
    // If the game state is gameplay and the NPC is not NULL, load the interaction
    if (*game_state == GAMEPLAY && npc != NULL){
        LoadInteraction(npc->base.dialoguePath, game_dialogue);
        if (game_dialogue->line_count > 0 || game_dialogue->choice_count > 0){
            *game_state = DIALOGUE_CUTSCENE;
            strncpy(current_interactable_id, npc->base.interactable_id, 63);
            game_dialogue->selected_choice = -1;
        }
    } else if (*game_state == DIALOGUE_CUTSCENE){
        // 1. Choice selection (1-4 keys)
        if (game_dialogue->choice_count > 0 && game_dialogue->current_line >= game_dialogue->line_count - 1){
            int key = 0;
            if (IsKeyPressed(KEY_ONE)) key = 1;
            else if (IsKeyPressed(KEY_TWO)) key = 2;
            else if (IsKeyPressed(KEY_THREE)) key = 3;
            else if (IsKeyPressed(KEY_FOUR)) key = 4;

            if (key > 0 && key <= game_dialogue->choice_count){
                // Get the index of the selected choice
                int idx = key - 1;
                // Clear the dialogue lines
                memset(game_dialogue->lines, 0, sizeof(game_dialogue->lines));
                // Copy the selected choice response to the dialogue lines
                strncpy(game_dialogue->lines[0], game_dialogue->choice_responses[idx], MAX_LINE_LENGTH-1);
                game_dialogue->line_count = 1;
                game_dialogue->current_line = 0;
                game_dialogue->choice_count = 0;
                game_dialogue->selected_choice = idx;
                
                // Apply Karma
                if (game_dialogue->choice_karma[idx] != 0){
                    UpdateAssetKarma(current_interactable_id, game_dialogue->choice_karma[idx]);
                }
                
                // Story Progression Check
                StoryPhase* active = GetActivePhase(&game_context->story);

                if (active && active->end_condition.type == CONDITION_INTERACT_OBJECT){
                    if (strcmp(active->end_condition.target_id, current_interactable_id) == 0 &&
                        ((int)active->end_condition.target_value == (idx + 1) || (int)active->end_condition.target_value == 0)){
                        if (game_dialogue->choice_ends[idx] || (int)active->end_condition.target_value == 0){
                            AdvanceStory(game_context);
                        }
                    }
                }
            }
            return; // blocks standard SPACE progression during choice selection
        }

        // 2. Standard dialogue progression (only on SPACE)
        if (IsKeyPressed(KEY_SPACE)){
            game_dialogue->current_line++;
            if (game_dialogue->current_line >= game_dialogue->line_count){
                *game_state = GAMEPLAY;
                game_dialogue->current_line = 0;

                // Check Story Condition for Object Interaction finish
                StoryPhase* active = GetActivePhase(&game_context->story);
                if (active && active->end_condition.type == CONDITION_INTERACT_OBJECT){
                    bool condition_met = false;
                    if (strcmp(active->end_condition.target_id, current_interactable_id) == 0){
                        int target_choice = (int)active->end_condition.target_value;
                        if (target_choice == 0){
                            condition_met = true;
                        } else if (game_dialogue->selected_choice == target_choice - 1){
                            if (game_dialogue->choice_ends[game_dialogue->selected_choice]){
                                condition_met = true;
                            }
                        }
                    }

                    if (condition_met){
                        AdvanceStory(game_context);
                    }
                }
            }
        }
    }
}

void InteractWithItem(Item *item, Dialogue *game_dialogue, GameState *game_state, Character *player, struct GameContext *game_context){
    // If the item is a pickup, add it to the player's inventory
    if (item->is_pickup){
        // Add the item to the player's inventory
        strcpy(player->inventory[player->inventory_count], item->base.texturePath);
        player->item_count[player->inventory_count]++;
        player->inventory_count++;
        item->picked_up = true;
    } else{
        // Load the interaction
        LoadInteraction(item->base.dialoguePath, game_dialogue);
        if (game_dialogue->line_count > 0 || game_dialogue->choice_count > 0){
            *game_state = DIALOGUE_CUTSCENE;
            strncpy(current_interactable_id, item->base.interactable_id, 63);
            game_dialogue->selected_choice = -1;
        }
    }
}

void InteractWithDoor(Door *door, Map *map, Character *player, struct GameContext *game_context){
    if (door == NULL || map == NULL || player == NULL || game_context == NULL) return;

    // Get the target map and location
    char targetMap[128];
    strncpy(targetMap, door->targetMapPath, 127);
    Location targetLoc = door->targetLocation;

    // Free the current map
    FreeMap(map);
    // Initialize the new map
    *map = InitMap(targetMap);
    // Set the player's position to the spawn position of the new map
    player->position = map->spawn_position;

    // Load static assets for this map
    LoadLocationAssets(targetLoc, game_context);
    
    // Also check if this map reload satisfies a phase change requirement
    game_context->location = targetLoc;
}