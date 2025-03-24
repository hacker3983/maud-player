#ifndef _MUSIC_SCROLLCONTAINER_H
#define _MUSIC_SCROLLCONTAINER_H
#include "music_player.h"

// Initialize the scroll container properties
void mplayer_scrollcontainer_setprops(music_scrollcontainer_t* container, SDL_Rect scroll_area, int scroll_speed);

// Append an new item to the scroll container. it must be initialized with with mplayer_scrollcontainer_init
void mplayer_scrollcontainer_appenditem(music_scrollcontainer_t* container, music_scrollcontaineritem_t scroll_item);

// Detects whether we hover over the scroll area where we should detect scroll
bool mplayer_scrollcontainer_hover_scrollarea(mplayer_t* mplayer, music_scrollcontainer_t container);

// Perform scrolling operation on a scroll container when we detect scroll within the scroll area
void mplayer_scrollcontainer_performscroll(mplayer_t* mplayer, music_scrollcontainer_t* container);

// Print out the scroll items that are in the scroll container
void mplayer_scrollcontainer_printinfo(music_scrollcontainer_t container);

// Destroy the memory allocated for the scroll container
void mplayer_scrollcontainer_destroy(music_scrollcontainer_t* container);
#endif