/*
This file contains the implementation of the game camera and 
camera-related logic.

Made by Andrew Zhuo.
*/

#include "game_context.h"
#include "raymath.h"

GameContext InitGameContext(Map* map, Character* player, Settings* settings){
    /* Initialize the game context */
    GameContext game_context = {0};

    // Initialize the camera.
    game_context.camera.offset = (Vector2){settings->window_width / 2.0f, settings->window_height / 2.0f};
    game_context.camera.target = player->position;
    game_context.camera.rotation = 0.0f;
    game_context.camera.zoom = 0.5f;

    // Initialize the map.
    game_context.map = map;

    // Initialize the player.
    game_context.player = player;

    // Initialize the outdoor status.
    game_context.is_outdoor = true;

    return game_context;
}

void UpdateGameContext(GameContext* game_context, Settings* settings, Vector2 map_size){
    /* Update the game context. */
    
    // Set camera target to follow player
    game_context->camera.target = game_context->player->position;
    
    // Zoom in based on hallucination level
    if (game_context->player->hallucination > game_context->player->max_hallucination * 0.9f){
        // Zoom increases from 1.0 at 90% of max hallucination
        float zoom_factor = (game_context->player->hallucination - game_context->player->max_hallucination * 0.9f) / (game_context->player->max_hallucination * 0.1f);
        game_context->camera.zoom += zoom_factor * 0.5f;
        
        // Cap zoom at 2.5x for playability
        if (game_context->camera.zoom > 2.5f) game_context->camera.zoom = 2.5f;
    }

    // Clamping camera target to map boundaries.
    game_context->camera.target.x = Clamp(
        game_context->camera.target.x,
        game_context->camera.offset.x / game_context->camera.zoom,
        map_size.x - (settings->window_width - game_context->camera.offset.x) / game_context->camera.zoom
    );
    game_context->camera.target.y = Clamp(
        game_context->camera.target.y,
        game_context->camera.offset.y / game_context->camera.zoom,
        map_size.y - (settings->window_height - game_context->camera.offset.y) / game_context->camera.zoom
    );
    
    // If map is smaller than window, keep the camera centered
    if (map_size.x < settings->window_width / game_context->camera.zoom){
        game_context->camera.target.x = map_size.x / 2.0f;
    }
    if (map_size.y < settings->window_height / game_context->camera.zoom){
        game_context->camera.target.y = map_size.y / 2.0f;
    }
}
