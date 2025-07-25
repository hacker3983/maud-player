#ifndef _MAUD_SCROLLCONTAINER_H
#define _MAUD_SCROLLCONTAINER_H
#include "maud_player.h"

// Initialize the scroll container
void maud_scrollcontainer_init(maud_scrollcontainer_t* container);

// set the scroll container content count
void maud_scrollcontainer_setcontent_count(maud_scrollcontainer_t* container, size_t content_count);

// set the scroll container scroll area / viewport
void maud_scrollcontainer_setscroll_area(maud_scrollcontainer_t* container, SDL_Rect scroll_area);

// set the scroll container scroll y position
void maud_scrollcontainer_setscroll_y(maud_scrollcontainer_t* container, int scroll_y);

// set the scroll container scroll y from the scroll area / viewport
void maud_scrollcontainer_setscroll_yfromscroll_area(maud_scrollcontainer_t* container);

// set the scroll container scroll speed
void maud_scrollcontainer_setscroll_speed(maud_scrollcontainer_t* container, int scroll_speed);

// set the scroll container properties
void maud_scrollcontainer_setprops(maud_scrollcontainer_t* container, SDL_Rect scroll_area,
    int scroll_speed, size_t content_count);

// Adds a new scroll item to the container
void maud_scrollcontainer_additem(maud_scrollcontainer_t* container, SDL_Rect scroll_item);

// Set the next item canvas
void maud_scrollcontainer_setnext_itemcanvas(maud_scrollcontainer_t* container, SDL_Rect canvas);

// Reset the item canvas index
void maud_scrollcontainer_resetitem_index(maud_scrollcontainer_t* container);

// Detects whether we hover over the scroll area where we should detect scroll
bool maud_scrollcontainer_hover_scrollarea(maud_t* maud, maud_scrollcontainer_t container);

// Perform scroll specifically over the screen area but let them disappear whenever item is scrolled below the scroll area
// y position
void maud_scrollcontainer_performscroll_overscrollarea(maud_t* maud, maud_scrollcontainer_t* container);

// Perform scrolling operation on a scroll container when we detect scroll within the scroll area
void maud_scrollcontainer_performscroll(maud_t* maud, maud_scrollcontainer_t* container, int scroll_y);

// Destroy the memory allocated for the scroll container
void maud_scrollcontainer_destroy(maud_scrollcontainer_t* container);
#endif