#include "maud_selectionmenu.h"

void maud_selectionmenu_renderer_init_menu(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    if(selection_menu->init) {
        return;
    }
    int start_x = 0, start_y = maud->music_searchbar.y + maud->music_searchbar.h;
    maud_selectionmenu_t new_menu = {
        .canvas = {
            .x = start_x, .y = start_y,
            .w = maud->win_width, .h = maud->music_searchbar.h
        },
        .color = {0x0E, 0x34, 0xA0, 0xFF},
        .songs_selectioninfo = {
            .font_size = 16,
            .text_color = {0},
            .text_canvas = {
                .x = 10
            }
        },
        .playbtn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .x = 10,
                .w = 24, .h = 30
            },
            .text = {
                .font_size = 16,
                .text = "Play",
                .text_color = white
            }
        },
        .playnextbtn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 24, .h = 30
            },
            .text = {
                .font_size = 16,
                .text = "Play next",
                .text_color = white
            }
        },
        .addtobtn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 16,
                .text = "Add to",
                .text_color = white
            }
        },
        .removebtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .color = black,
            .icon_canvas = {
                .w = 24, .h = 24
            },
            .text = {
                .font_size = 18,
                .text = "Remove",
                .text_color = white
            }
        },
        .moveup_btn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 16,
                .text = "Move up",
                .text_color = white
            }
        },
        .movedown_btn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 16,
                .text = "Move down",
                .text_color = white
            }
        }
    };
    new_menu.init = true;
    *selection_menu = new_menu;
}

void maud_selectionmenu_renderer_init(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_renderer_init_menu(maud, selection_menu);
}

void maud_selectionmenu_renderer_display(maud_t* maud);