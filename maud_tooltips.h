#ifndef _MUSIC_TOOLTIPS_H
#define _MUSIC_TOOLTIPS_H
#include "maud_player.h"
// Gets the text content of the playlist based on how its properties are set up
void maud_tooltip_getcontents(maud_tooltip_t* tooltip, SDL_Rect tooltip_canvas);
// Gets the size of a tooltip without rendering it
void maud_tooltip_getsize(maud_tooltip_t* tooltip);
// Renders the tooltip to the screen only when you hover over its element
void maud_tooltip_renderhover(maud_t* maud, maud_tooltip_t* tooltip);
// Renders the tooltip to the screen even when you don't hover over its element
void maud_tooltip_render(maud_t* maud, maud_tooltip_t* tooltip);
// Display the contents of a tooltip on screen this is used by the tooltip render function
void maud_tooltip_displaycontents(maud_t* maud, maud_tooltip_t* tooltip);
// Prints the contents of a tooltip
void maud_tooltip_printcontents(maud_tooltip_t* tooltip);
// Destroys the text contents of the tooltip
void maud_tooltip_destroycontents(maud_tooltip_t* tooltip);
#endif