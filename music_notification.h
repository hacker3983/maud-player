#ifndef _MUSIC_NOTIFICATION_H
#define _MUSIC_NOTIFICATION_H
#include "music_player.h"

// Adds a notification to the notification queue
void mplayer_notification_push(music_notification_t* notification, TTF_Font* font, int font_size, SDL_Color background_color,
    const char* message, SDL_Color message_color, double timeout_ms, int padding_x, int padding_y);
// Removes the timed out notification from the notification queue
void mplayer_notification_pop(music_notification_t* notification);
// Display the most recent notification
void mplayer_notification_display(mplayer_t* mplayer, music_notification_t* notification);
// Release all the memory used by the notification system back to the system
void mplayer_notification_destroy(music_notification_t* notification);
#endif