/*
This file contains the phone system implementation for notifications and messages.

Made by Andrew Zhuo.
*/

#include "phone.h"
#include <string.h>

void InitPhone(Phone *phone){
    /* Initialize the phone */
    phone->state = PHONE_IDLE;
    phone->notif_timer = 0;
    memset(phone->message, 0, sizeof(phone->message));
    memset(phone->replies, 0, sizeof(phone->replies));
    memset(phone->selected_reply, 0, sizeof(phone->selected_reply));
}

void TriggerPhoneNotification(Phone *phone, const char *msg, const char *reply1, const char *reply2){
    /* Trigger a new notification */
    strncpy(phone->message, msg, sizeof(phone->message) - 1);
    strncpy(phone->replies[0], reply1, sizeof(phone->replies[0]) - 1);
    strncpy(phone->replies[1], reply2, sizeof(phone->replies[1]) - 1);

    // Set notification timer to 5 seconds
    phone->notif_timer = 5.0f;
    phone->state = PHONE_NOTIFICATION;
}

void UpdatePhone(Phone *phone, float delta){
    /* Update the phone */
    if (phone->state == PHONE_NOTIFICATION){
        // Decrease notification timer
        phone->notif_timer -= delta;
        if (phone->notif_timer <= 0){
            // Close notification
            phone->state = PHONE_IDLE;
        }
    }
}

void DrawPhone(Phone *phone){
    /* Draw the phone */
    if (phone->state == PHONE_IDLE) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (phone->state == PHONE_NOTIFICATION){
        // Draw small notification box at bottom right
        int boxWidth = 300;
        int boxHeight = 60;
        int margin = 20;
        Rectangle box = {
            (float)screenWidth - boxWidth - margin,
            (float)screenHeight - boxHeight - margin,
            (float)boxWidth,
            (float)boxHeight
        };
        
        DrawRectangleRec(box, Fade(BLACK, 0.8f));
        DrawRectangleLinesEx(box, 2, GOLD);
        DrawText("New Message! Press 'R'", box.x + 10, box.y + 20, 20, WHITE);
    } else if (phone->state == PHONE_OPENED || phone->state == PHONE_SHOWING_REPLY){
        // Draw Phone UI
        int phoneWidth = 400;
        int phoneHeight = 600;
        Rectangle phoneBox = {
            (float)screenWidth / 2.0f - phoneWidth / 2.0f,
            (float)screenHeight / 2.0f - phoneHeight / 2.0f,
            (float)phoneWidth,
            (float)phoneHeight
        };

        // Phone frame
        DrawRectangleRec(phoneBox, DARKGRAY);
        DrawRectangleLinesEx(phoneBox, 4, BLACK);
        
        // Header
        DrawRectangle(phoneBox.x, phoneBox.y, phoneBox.width, 40, BLACK);
        DrawText("PHONE", phoneBox.x + phoneBox.width / 2.0f - MeasureText("PHONE", 20) / 2.0f, phoneBox.y + 10, 20, RAYWHITE);

        // Screen area
        Rectangle screen = { phoneBox.x + 10, phoneBox.y + 50, phoneBox.width - 20, phoneBox.height - 180 };
        DrawRectangleRec(screen, Fade(SKYBLUE, 0.2f));
        DrawText(phone->message, screen.x + 20, screen.y + 20, 20, WHITE);

        if (phone->state == PHONE_OPENED){
            /* Commented out reply choice system
            int btnHeight = 50;
            Rectangle btn1 = {phoneBox.x + 20, phoneBox.y + phoneBox.height - 120, phoneBox.width - 40, (float)btnHeight};
            Rectangle btn2 = {phoneBox.x + 20, phoneBox.y + phoneBox.height - 60, phoneBox.width - 40, (float)btnHeight};

            // Button 1
            bool hover1 = CheckCollisionPointRec(GetMousePosition(), btn1);
            DrawRectangleRec(btn1, hover1 ? LIGHTGRAY : GRAY);
            DrawRectangleLinesEx(btn1, 2, hover1 ? YELLOW : BLACK);
            DrawText(TextFormat("1: %s", phone->replies[0]), btn1.x + 10, btn1.y + 15, 20, BLACK);

            // Button 2
            bool hover2 = CheckCollisionPointRec(GetMousePosition(), btn2);
            DrawRectangleRec(btn2, hover2 ? LIGHTGRAY : GRAY);
            DrawRectangleLinesEx(btn2, 2, hover2 ? YELLOW : BLACK);
            DrawText(TextFormat("2: %s", phone->replies[1]), btn2.x + 10, btn2.y + 15, 20, BLACK);
            */
            
            // Simplified Enter Reply Prompt
            DrawText("Press ENTER to reply", phoneBox.x + 20, phoneBox.y + phoneBox.height - 80, 20, GREEN);
        } else if (phone->state == PHONE_SHOWING_REPLY){
            // Show selected reply
            DrawText("Me:", screen.x + 20, screen.y + 100, 15, GREEN);
            DrawText(phone->selected_reply, screen.x + 20, screen.y + 120, 20, WHITE);
        }
    }
}

void HandlePhoneInput(Phone *phone){
    /* Handle phone input */
    if (phone->state == PHONE_IDLE) return;

    if (phone->state == PHONE_NOTIFICATION){
        if (IsKeyPressed(KEY_R)){
            phone->state = PHONE_OPENED;
            return;            // Prevent closing in the same frame
        }
    } else if (phone->state == PHONE_OPENED){
        // Fixed reply with ENTER
        if (IsKeyPressed(KEY_ENTER)){
            strncpy(phone->selected_reply, phone->replies[0], sizeof(phone->selected_reply) - 1);
            phone->state = PHONE_SHOWING_REPLY;
        }

        /* Commented out selection logic for now
        if (IsKeyPressed(KEY_ONE)){
            strncpy(phone->selected_reply, phone->replies[0], sizeof(phone->selected_reply) - 1);
            phone->state = PHONE_SHOWING_REPLY;
        }
        if (IsKeyPressed(KEY_TWO)){
            strncpy(phone->selected_reply, phone->replies[1], sizeof(phone->selected_reply) - 1);
            phone->state = PHONE_SHOWING_REPLY;
        }
        */
    }

    // Global close for any phone state except when just opened from notification
    if (IsKeyPressed(KEY_R)){
        phone->state = PHONE_IDLE;
    }
}
