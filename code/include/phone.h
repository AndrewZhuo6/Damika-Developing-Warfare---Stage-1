/*
This file contains the phone system for notifications and messages.

Made by Andrew Zhuo.
*/

#ifndef PHONE_H
#define PHONE_H

#include "raylib.h"

typedef enum {
    /* Phone states */
    PHONE_IDLE,             // Phone is closed
    PHONE_NOTIFICATION,     // Phone is showing a notification
    PHONE_OPENED,           // Phone is open
    PHONE_SHOWING_REPLY     // Phone is showing a reply
} PhoneState;

typedef struct {
    /* Phone data */
    char message[128];                 // Message to be displayed
    char replies[2][64];               // Replies to the message
    char selected_reply[64];           // Selected reply
    PhoneState state;                  // Phone state
    float notif_timer;                 // Notification timer
} Phone;

void InitPhone(Phone *phone);          // Initialize the phone system

// Trigger a new notification
void TriggerPhoneNotification(
    Phone *phone,
    const char *msg,
    const char *reply1,
    const char *reply2
);

void UpdatePhone(Phone *phone, float delta);             // Update phone state (timer, etc.)
void DrawPhone(Phone *phone);                            // Draw phone UI
void HandlePhoneInput(Phone *phone);                     // Handle phone input

#endif
