/**
 * @file interactive.c
 * @brief Implementation of the UI system (Buttons and Sliders).
 * 
 * Handles layout calculations based on screen state and processes
 * mouse interaction for menus and settings.
 * 
 * Authors: Andrew Zhuo
 */

#include "interactive.h"
#include "raylib.h"
#include "raymath.h"

Interactive InitInteractive(Settings* game_settings){
    Interactive new_interactive = {0};

    // 1. Resource Loading
    new_interactive.new_game_button = LoadTexture("../assets/images/buttons/new_game.png");
    new_interactive.continue_button = LoadTexture("../assets/images/buttons/continue.png");
    new_interactive.main_menu_button = LoadTexture("../assets/images/buttons/main_menu.png");
    new_interactive.settings_button = LoadTexture("../assets/images/buttons/settings.png");
    new_interactive.quit_button = LoadTexture("../assets/images/buttons/quit.png");

    // 2. Geometry Setup (Constants)
    new_interactive.bar_width = 770.0f;
    new_interactive.bar_height = 5.0f;
    new_interactive.knob_width = 100.0f;
    new_interactive.knob_height = 120.0f;

    // 3. Initial Layout pass
    UpdateInteractiveLayout(&new_interactive, MAINMENU, game_settings);
    
    return new_interactive;
}

void UpdateInteractiveLayout(Interactive* interactive, int game_state, Settings* game_settings){
    float screen_width = (float)GetScreenWidth();
    float screen_height = (float)GetScreenHeight();
    
    // 1. Calculate scale factors relative to the original reference resolution (1200x800)
    const float ref_w = (float)game_settings->window_width;
    const float ref_h = (float)game_settings->window_height;
    float scale_x = screen_width / ref_w;
    float scale_y = screen_height / ref_h;

    // 2. Scale the base layout units
    float slot_h = (float)interactive->new_game_button.height * scale_y;
    float button_spacing = 20.0f * scale_y;
    
    // 3. Calculate start_y using the scaled height for centering
    float total_h = 4 * slot_h + 3 * button_spacing;
    float start_y = (screen_height / 2.0f) - (total_h / 2.0f);

    if (game_state == MAINMENU){
        // Original manual adjustments were made on 1200x800
        // We use scale_x and scale_y to keep these adjustments proportional

        if (FileExists("../data/data.dat")){
            interactive->continue_bounds = (Rectangle){
                screen_width / 2.0f - (float)interactive->new_game_button.width * 1.9f * scale_x, 
                start_y + (float)interactive->new_game_button.height * 0.6f * scale_y, 
                (float)interactive->new_game_button.width * 1.9f * scale_x, 
                slot_h * 2.0f 
            };
            interactive->new_game_bounds = (Rectangle){
                screen_width / 2.0f + (float)interactive->continue_button.width * 0.15f * scale_x, 
                start_y + (float)interactive->continue_button.height * 0.6f * scale_y, 
                (float)interactive->continue_button.width * 1.9f * scale_x, 
                slot_h * 2.0f
            };
        } else{
            interactive->new_game_bounds = (Rectangle){
                screen_width / 2.0f - (float)interactive->new_game_button.width * 0.95f * scale_x, 
                start_y + (float)interactive->new_game_button.height * 0.6f * scale_y, 
                (float)interactive->new_game_button.width * 1.9f * scale_x, 
                slot_h * 2.0f
            };
            interactive->continue_bounds = (Rectangle){0, 0, 0, 0};
        }
        interactive->settings_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->settings_button.width * 0.95f * scale_x, 
            start_y + 2.6f * (slot_h + button_spacing), 
            (float)interactive->settings_button.width * 1.9f * scale_x, 
            slot_h * 2.0f
        };
        interactive->quit_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->quit_button.width * 0.95f * scale_x, 
            start_y + 4.7f * (slot_h + button_spacing), 
            (float)interactive->quit_button.width * 1.9f * scale_x, 
            slot_h * 2.0f
        };
    } else if (game_state == PAUSE){
        // Scale standard pause menu buttons to maintain original proportions
        float pause_btn_w = (float)interactive->continue_button.width * scale_x;
        float pause_btn_h = slot_h;

        interactive->continue_bounds = (Rectangle){
            screen_width / 2.0f - pause_btn_w / 2.0f, 
            start_y, 
            pause_btn_w, 
            pause_btn_h 
        };
        interactive->settings_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->settings_button.width * scale_x / 2.0f, 
            start_y + 1 * (slot_h + button_spacing), 
            (float)interactive->settings_button.width * scale_x, 
            slot_h 
        };
        interactive->main_menu_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->main_menu_button.width * scale_x / 2.0f, 
            start_y + 2 * (slot_h + button_spacing), 
            (float)interactive->main_menu_button.width * scale_x, 
            slot_h 
        };
        interactive->quit_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->quit_button.width * scale_x / 2.0f, 
            start_y + 3 * (slot_h + button_spacing), 
            (float)interactive->quit_button.width * scale_x, 
            slot_h 
        };
    }

    float bar_w = interactive->bar_width * scale_x;
    float bar_h = interactive->bar_height * scale_y;
    interactive->volume_slider_bar = (Rectangle){ 
        screen_width / 2.0f - bar_w / 2.0f, 
        screen_height / 2.0f + bar_h * 14.0f, 
        bar_w, 
        bar_h 
    };

    // Reference Knob dimensions (100x120)
    interactive->knob_width = interactive->knob_width * scale_x;
    interactive->knob_height = interactive->knob_height * scale_y;

    // Settings Back Button (460x115 reference)
    float btn_w = 460.0f * scale_x;
    float btn_h = 115.0f * scale_y;
    interactive->settings_back_bounds = (Rectangle){
        screen_width / 2.0f - btn_w / 2.0f,
        screen_height / 2.0f + btn_h * 1.75f,
        btn_w,
        btn_h
    };
}

void UpdateInteractive(Interactive* interactive, Settings* game_settings){
    Vector2 mouse_position = GetMousePosition();

    // Reset clicked triggers (latches)
    interactive->is_new_game_clicked = false;
    interactive->is_continue_clicked = false;
    interactive->is_main_menu_clicked = false;
    interactive->is_settings_clicked = false;
    interactive->is_quit_clicked = false;
    interactive->is_settings_back_clicked = false;

    // --- Phase 1: Button Interaction ---
    interactive->is_new_game_hovered = CheckCollisionPointRec(mouse_position, interactive->new_game_bounds);
    interactive->is_continue_hovered = CheckCollisionPointRec(mouse_position, interactive->continue_bounds);
    interactive->is_main_menu_hovered = CheckCollisionPointRec(mouse_position, interactive->main_menu_bounds);
    interactive->is_settings_hovered = CheckCollisionPointRec(mouse_position, interactive->settings_bounds);
    interactive->is_quit_hovered = CheckCollisionPointRec(mouse_position, interactive->quit_bounds);
    interactive->is_settings_back_hovered = CheckCollisionPointRec(mouse_position, interactive->settings_back_bounds);

    if (interactive->is_new_game_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_new_game_clicked = true;
    if (interactive->is_continue_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_continue_clicked = true;
    if (interactive->is_main_menu_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_main_menu_clicked = true;
    if (interactive->is_settings_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_settings_clicked = true;
    if (interactive->is_quit_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_quit_clicked = true;
    if (interactive->is_settings_back_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_settings_back_clicked = true;

    // --- Phase 2: Slider Interaction ---
    // 1. Calculate knob position based on current settings (percentage mapped to width)
    float knob_x = interactive->volume_slider_bar.x + (game_settings->game_volume * interactive->volume_slider_bar.width / 100.0f);
    
    interactive->volume_slider_knob = (Rectangle){
        knob_x - interactive->knob_width / 2.0f,
        interactive->volume_slider_bar.y + interactive->volume_slider_bar.height / 2.0f - interactive->knob_height / 2.0f,
        interactive->knob_width,
        interactive->knob_height
    };

    // 2. State transition: Drag Start
    if (CheckCollisionPointRec(mouse_position, interactive->volume_slider_knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        interactive->is_volume_moving = true;
    }

    // 3. State transition: Drag Stop
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) interactive->is_volume_moving = false;

    // 4. Transform Logic: Dragging
    if (interactive->is_volume_moving){
        float new_knob_x = mouse_position.x;
        new_knob_x = Clamp(new_knob_x, interactive->volume_slider_bar.x, interactive->volume_slider_bar.x + interactive->volume_slider_bar.width);

        // Map x-position back to 0-100 range
        game_settings->game_volume = (new_knob_x - interactive->volume_slider_bar.x) / interactive->volume_slider_bar.width * 100.0f;
        
        // Immediate Feedback: update Raylib master volume
        SetMasterVolume(game_settings->game_volume / 100.0f);
        
        // Visual sync
        interactive->volume_slider_knob.x = new_knob_x - interactive->knob_width / 2.0f;
    }
}

void CloseInteractive(Interactive* interactive){
    // Unload all textures from VRAM
    UnloadTexture(interactive->new_game_button);
    UnloadTexture(interactive->continue_button);
    UnloadTexture(interactive->main_menu_button);
    UnloadTexture(interactive->settings_button);
    UnloadTexture(interactive->quit_button);
}