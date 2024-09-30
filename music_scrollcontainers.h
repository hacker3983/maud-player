#ifndef _MUSIC_SETTINGSMENU_SCROLL_H
#define _MUSIC_SETTINGSMENU_SCROLL_H
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct music_player_scrollcontainer_areainfo {
    int mouse_x, mouse_y; // The x and y coordinates of the mouse
    SDL_Rect scroll_area;
} mplayer_scrollcontainer_areainfo_t;

typedef struct music_playerscroll_container {
    SDL_Rect scroll_canvas; // The canvas that we should scroll on
    SDL_Rect initial_canvas; // The intial position for the canvas this will be used to restore the canvas position
    int disappear_y; // The y position on the screen to disappear or stop rendering the
                     // content on screen
    bool vertical_scroll; // True for vertical scroll and False for horizontal scroll
    bool render;
} mplayer_scrollcontainer_t;

mplayer_scrollcontainer_t mplayer_scrollcontainer_create(SDL_Rect initial_canvas, int disappear_y,
    bool vertical_scroll, bool render);
void mplayer_scrollcontainer_append(mplayer_scrollcontainer_t** containers,
    mplayer_scrollcontainer_t container, size_t* container_count);
bool mplayer_scrollcontainer_hover_scrollarea(mplayer_scrollcontainer_areainfo_t scrollarea_info);
void mplayer_scrollcontainer_performscroll(mplayer_scrollcontainer_t* container, int scroll_type, int scroll_speed,
    mplayer_scrollcontainer_areainfo_t scrollarea_info);
void mplayer_scrollcontainers_performscroll(mplayer_scrollcontainer_t* containers, int scroll_type, int scroll_speed,
    size_t container_count, mplayer_scrollcontainer_areainfo_t scrollarea_info);
void mplayer_scrollcontainer_printinfo(mplayer_scrollcontainer_t container);
void mplayer_scrollcontainer_destroylist(mplayer_scrollcontainer_t** containers, size_t* container_count);
#endif