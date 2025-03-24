#ifndef _MUSIC_TOOLTIPS_H
#define _MUSIC_TOOLTIPS_H
#include "music_player.h"
// Gets the text content of the playlist based on how its properties are set up
void mplayer_tooltip_getcontents(mplayer_tooltip_t* tooltip, SDL_Rect tooltip_canvas);
// Gets the size of a tooltip without rendering it
void mplayer_tooltip_getsize(mplayer_tooltip_t* tooltip);
// Renders the tooltip to the screen only when you hover over its element
void mplayer_tooltip_renderhover(mplayer_t* mplayer, mplayer_tooltip_t* tooltip);
// Renders the tooltip to the screen even when you don't hover over its element
void mplayer_tooltip_render(mplayer_t* mplayer, mplayer_tooltip_t* tooltip);
// Display the contents of a tooltip on screen this is used by the tooltip render function
void mplayer_tooltip_displaycontents(mplayer_t* mplayer, mplayer_tooltip_t* tooltip);
// Prints the contents of a tooltip
void mplayer_tooltip_printcontents(mplayer_tooltip_t* tooltip);
// Destroys the text contents of the tooltip
void mplayer_tooltip_destroycontents(mplayer_tooltip_t* tooltip);
#endif