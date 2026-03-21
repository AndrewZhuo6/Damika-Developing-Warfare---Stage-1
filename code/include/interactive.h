/**
 * @file interactive.h
 * @brief UI elements and interactive menu system.
 * 
 * Handles buttons, sliders, and other graphical user interface elements
 * for the main menu, settings, and pause screens.
 * 
 * Authors: Andrew Zhuo
 */

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "raylib.h"
#include "settings.h"
#include "state.h"

/**
 * @brief Container for all UI elements and their interaction states.
 */
typedef struct Interactive {
    // --- Button Textures ---
    Texture2D new_game_button;   // Texture for 'New Game'
    Texture2D continue_button;   // Texture for 'Continue'
    Texture2D main_menu_button;  // Texture for 'Main Menu'
    Texture2D settings_button;   // Texture for 'Settings'
    Texture2D quit_button;       // Texture for 'Quit'

    // --- Button Layout ---
    Rectangle new_game_bounds;   // Hitbox for 'New Game' button
    Rectangle continue_bounds;   // Hitbox for 'Continue' button
    Rectangle main_menu_bounds;  // Hitbox for 'Main Menu' button
    Rectangle settings_bounds;   // Hitbox for 'Settings' button
    Rectangle quit_bounds;       // Hitbox for 'Quit' button

    // --- Interaction State ---
    bool is_new_game_clicked;    // True if button was clicked this frame
    bool is_continue_clicked;    
    bool is_main_menu_clicked;   
    bool is_settings_clicked;    
    bool is_quit_clicked;        

    bool is_new_game_hovered;    // True if mouse is over button
    bool is_continue_hovered;    
    bool is_main_menu_hovered;   
    bool is_settings_hovered;    
    bool is_quit_hovered;        

    // --- Volume Slider ---
    Rectangle volume_slider_bar;   // Visual bar of the slider
    Rectangle volume_slider_knob;  // Draggable handle of the slider
    Rectangle settings_back_bounds; // Hitbox for 'Back' button in settings
    float bar_width;               
    float bar_height;              
    float knob_width;              
    float knob_height;             
    bool is_volume_moving;         // True if user is currently dragging the knob
    bool is_settings_back_clicked; // True if back was clicked
    bool is_settings_back_hovered; // True if back is hovered
} Interactive;

/**
 * @brief Loads UI textures and initializes default button positions.
 * @param game_settings Pointer to settings for initial layout.
 * @return An initialized Interactive structure.
 */
Interactive InitInteractive(Settings* game_settings);

/**
 * @brief Updates hover and click states based on mouse position/input.
 * @param interactive Pointer to the UI container.
 * @param game_settings Pointer to settings.
 */
void UpdateInteractive(Interactive* interactive, Settings* game_settings);

/**
 * @brief Adjusts UI layout based on current game state (e.g., resizing window).
 * @param interactive Pointer to the UI container.
 * @param game_state Current state to determine which buttons to show.
 */
void UpdateInteractiveLayout(Interactive* interactive, int game_state, Settings* game_settings);

/**
 * @brief Unloads UI textures and frees resources.
 * @param interactive Pointer to the UI container to clean up.
 */
void CloseInteractive(Interactive* interactive);

#endif