/*
This file contains the implementation of the game camera and
camera-related logic.

Made by Andrew Zhuo.
*/

#include "game_context.h"
#include "raymath.h"

GameContext InitGameContext(Map *map, Character *player, Settings *settings) {
  /* Initialize the game context */
  GameContext game_context = {0};

  // Calculate initial dynamic zoom to maintain consistent field of view across
  // different resolutions Baseline: 2200 units visible width regardless of
  // window size
  float baseline_visible_width = 2200.0f;
  float initial_zoom = (float)GetScreenWidth() / baseline_visible_width;

  // Initialize the camera.
  game_context.camera.offset = (Vector2){(float)GetScreenWidth() / 2.0f,
                                         (float)GetScreenHeight() / 2.0f};
  game_context.camera.target = player->position;
  game_context.camera.rotation = 0.0f;
  game_context.camera.zoom = initial_zoom;

  // Initialize the map.
  game_context.map = map;

  // Initialize the player.
  game_context.player = player;

  // Initialize the outdoor status.
  game_context.is_outdoor = true;

  // Initialize the phone system.
  InitPhone(&game_context.phone);

  return game_context;
}

void UpdateGameContext(GameContext *game_context, Settings *settings,
                       Vector2 map_size) {
  /* Update the game context. */

  // Set camera target to follow player
  game_context->camera.target = game_context->player->position;

  // Update camera offset and zoom in case of window resize or fullscreen toggle
  float screen_width = (float)GetScreenWidth();
  float screen_height = (float)GetScreenHeight();

  game_context->camera.offset =
      (Vector2){screen_width / 2.0f, screen_height / 2.0f};

  // Calculate dynamic base zoom based on baseline visible units
  float baseline_visible_width = 2200.0f;
  float base_zoom = screen_width / baseline_visible_width;
  game_context->camera.zoom = base_zoom;

  // Zoom in based on hallucination level
  if (game_context->player->hallucination >
      game_context->player->max_hallucination * 0.9f) {
    // Zoom increase factor
    float zoom_increase = (game_context->player->hallucination -
                           game_context->player->max_hallucination * 0.9f) /
                          (game_context->player->max_hallucination * 0.1f) *
                          0.5f;
    game_context->camera.zoom = base_zoom + zoom_increase;

    // Cap zoom for playability
    if (game_context->camera.zoom > base_zoom + 1.5f)
      game_context->camera.zoom = base_zoom + 1.5f;
  }

  // Clamping camera target to map boundaries.
  game_context->camera.target.x =
      Clamp(game_context->camera.target.x,
            game_context->camera.offset.x / game_context->camera.zoom,
            map_size.x - (screen_width - game_context->camera.offset.x) /
                             game_context->camera.zoom);
  game_context->camera.target.y =
      Clamp(game_context->camera.target.y,
            game_context->camera.offset.y / game_context->camera.zoom,
            map_size.y - (screen_height - game_context->camera.offset.y) /
                             game_context->camera.zoom);

  // If map is smaller than window, keep the camera centered
  if (map_size.x < screen_width / game_context->camera.zoom) {
    game_context->camera.target.x = map_size.x / 2.0f;
  }
  if (map_size.y < screen_height / game_context->camera.zoom) {
    game_context->camera.target.y = map_size.y / 2.0f;
  }
}
