/*
This file contains the logic for the game state.

Made by Steven Kenneth Darwy
*/

#include "state.h"
#include "phone.h"

int UpdateGame(GameState* game_state, Interactive* game_interactive, 
    Character* player, Settings* game_settings, Map* game_map, 
    GameContext* game_context, Audio* game_audio, Vector2 map_size){
    /* Update the game state */
    switch(*game_state){
        case MAINMENU:
            UpdateInteractive(game_interactive, game_settings);
            if (game_interactive->is_play_clicked){
                *game_state = GAMEPLAY;
            }
            if (game_interactive->is_settings_clicked){
                *game_state = SETTINGS;
            }
            if (game_interactive->is_quit_clicked){
                return 1;
            }
            ShowCursor();
            break;
        case GAMEPLAY:
            if (game_context->phone.state != PHONE_OPENED && game_context->phone.state != PHONE_SHOWING_REPLY){
                UpdateCharacter(player, game_settings, map_size, game_map, game_audio, game_context->is_outdoor);
                UpdateGameContext(game_context, game_settings, map_size);

                if (player->position.x == 200) PlayScream(game_audio);

                // Trigger notification when passing x = 900
                if ((int)player->position.x >= 900 && game_context->phone.state == PHONE_IDLE){
                    PlayNotif(game_audio);
                    TriggerPhoneNotification(&game_context->phone, "You have new message", "Help me!", "I'm busy");
                }
            }

            HandlePhoneInput(&game_context->phone);

            if (game_context->phone.state == PHONE_OPENED || game_context->phone.state == PHONE_SHOWING_REPLY){
                ShowCursor();
            } else{
                HideCursor();
            }
            break;
        case PAUSE:
            UpdateInteractive(game_interactive, game_settings);
            if (game_interactive->is_play_clicked){
                *game_state = GAMEPLAY;
            }
            if (game_interactive->is_settings_clicked){
                *game_state = SETTINGS;
            }
            if (game_interactive->is_quit_clicked){
                return 1;
            }
            ShowCursor();
            break;
        case SETTINGS:
            UpdateInteractive(game_interactive, game_settings);
            if (IsKeyPressed(KEY_ESCAPE)){
                *game_state = MAINMENU;
            }
            ShowCursor();
            break;
        case DIALOGUE_CUTSCENE:
            HideCursor();
            break;
        default:
            break;
    }

    return 0;
}