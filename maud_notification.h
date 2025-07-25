#ifndef _MUSIC_NOTIFICATION_H
#define _MUSIC_NOTIFICATION_H
#include "maud_player.h"

// Adds a notification to the notification queue
void maud_notification_push(maud_notification_t* notification, TTF_Font* font, int font_size, SDL_Color background_color,
    const char* message, SDL_Color message_color, double timeout_secs, int padding_x, int padding_y,
    int message_spacing);
// Gets the size of the notification canvas
void maud_notification_getsize(maud_notification_t* notification);
// Adds a message segment for the top notification message
bool maud_notification_addmsgsegment(maud_notification_t* notification, text_info_t message_segment);
// Clear the current message segments for the top notification message
void maud_notification_clearsegment(maud_notification_t* notification);
// Removes the timed out notification from the notification queue
void maud_notification_pop(maud_notification_t* notification);
// Display the most recent notification
void maud_notification_display(maud_t* maud, maud_notification_t* notification);
// Release all the memory used by the notification system back to the system
void maud_notification_destroy(maud_notification_t* notification);
#endif