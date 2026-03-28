/**
 * @file story.h
 * @brief Defines the data structures for story sets and phases.
 * 
 * This file is included by other modules that need to use the story system.
 * 
 * Authors: Andrew Zhuo
 */

#ifndef STORY_H
#define STORY_H

#include "quest.h"

struct GameContext;

#define MAX_QUESTS_PER_PHASE 10
#define MAX_PHASES_PER_SET 10
#define MAX_SETS_PER_DAY 20

/**
 * @brief Types of conditions that can end a story phase.
 */
typedef enum StoryConditionType {
    CONDITION_ALL_QUESTS_COMPLETE,    // Advance when all phase quests are done
    CONDITION_INTERACT_OBJECT,        // Advance when specific object interacted
    CONDITION_TIME_PASS,              // Advance when time passes
    CONDITION_ENTER_LOCATION,         // Advance when entering a specific location
} StoryConditionType;

/**
 * @brief Types of locations in the game.
 */
typedef enum {
    STORY_LOC_NONE = -1,             // No location
    STORY_LOC_APARTMENT = 0,         // Apartment location
    STORY_LOC_EXTERIOR = 1,          // Exterior location
    STORY_LOC_INTERIOR = 2,          // Interior location
    STORY_LOC_FARM = 3,              // Farm location
    STORY_LOC_FOREST = 4,            // Forest location
} StoryLocation;

/**
 * @brief Represents the requirement to exit the current phase.
 */
typedef struct StoryCondition {
    StoryConditionType type;         // Type of condition
    char target_id[64];              // e.g., "fridge" or "farmer"
    float target_value;              // e.g., timer duration or expected choice index
} StoryCondition;

/**
 * @brief Represents an interactable object in the game.
 */
typedef struct {
    char id[64];       // ID of the interactable object
    char type[32];     // "ITEM" or "NPC" or "DOOR"
} PhaseInteractable;

/**
 * @brief A single narrative step with its own objectives and exit condition.
 */
typedef struct StoryPhase {
    char name[64];                           // Name of the phase
    StoryLocation location;                  // Location of the phase
    Quest quests[MAX_QUESTS_PER_PHASE];      // Quests in the phase
    int quest_count;                         // Number of quests in the phase
    PhaseInteractable interactables[20];     // Interactable objects in the phase
    int interactable_count;                  // Number of interactable objects in the phase
    StoryCondition end_condition;            // Condition to end the phase
} StoryPhase;

/**
 * @brief A collection of phases grouped by scene or narrative beat.
 */
typedef struct StorySet {
    char name[64];                             // Name of the set
    StoryPhase phases[MAX_PHASES_PER_SET];     // Phases in the set
    int phase_count;                           // Number of phases in the set
} StorySet;

/**
 * @brief The top-level state for the current day's progression.
 */
typedef struct StorySystem {
    StorySet sets[MAX_SETS_PER_DAY];        // Sets in the day
    int set_count;                          // Number of sets in the day
    int current_set_idx;                    // Index of the current set
    int current_phase_idx;                  // Index of the current phase
    float phase_timer;                      // Timer for TIME_PASS conditions
    char day_folder[32];                    // e.g., "day1"
} StorySystem;

/**
 * @brief Parses a Day text file and populates the StorySystem.
 *
 * @param story Pointer to the StorySystem to populate.
 * @param path Path to the Day text file.
 */
void LoadStoryDay(StorySystem* story, const char* path);

/**
 * @brief Checks phase progression every frame.
 *
 * @param game_context Pointer to the GameContext.
 * @param delta Time elapsed since the last frame.
 */
void UpdateStory(struct GameContext* game_context, float delta);

/**
 * @brief Force-advances the story to the next phase.
 *
 * @param game_context Pointer to the GameContext.
 */
void AdvanceStory(struct GameContext* game_context);

/**
 * @brief Returns a pointer to the current active phase.
 *
 * @param story Pointer to the StorySystem.
 * @return StoryPhase* Pointer to the current active phase.
 */
StoryPhase* GetActivePhase(StorySystem* story);

#endif
