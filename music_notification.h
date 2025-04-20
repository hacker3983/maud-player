#ifndef _MUSIC_NOTIFICATION_H
#define _MUSIC_NOTIFICATION_H
#include "music_player.h"

// Adds a notification to the notification queue
void mplayer_notification_push(music_notification_t* notification, TTF_Font* font, int font_size, SDL_Color background_color,
    const char* message, SDL_Color message_color, double timeout_secs, int padding_x, int padding_y,
    int message_spacing);
// Gets the size of the notification canvas
void mplayer_notification_getsize(music_notification_t* notification);
// Adds a message segment for the top notification message
bool mplayer_notification_addmsgsegment(music_notification_t* notification, text_info_t message_segment);
// Clear the current message segments for the top notification message
void mplayer_notification_clearsegment(music_notification_t* notification);
// Removes the timed out notification from the notification queue
void mplayer_notification_pop(music_notification_t* notification);
// Display the most recent notification
void mplayer_notification_display(mplayer_t* mplayer, music_notification_t* notification);
// Release all the memory used by the notification system back to the system
void mplayer_notification_destroy(music_notification_t* notification);
#endif