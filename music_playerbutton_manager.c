#include "music_playerbutton_manager.h"

bool mplayer_buttonmanager_ibutton_hover(mplayer_t* mplayer, ibtn_t button) {
    return mplayer_rect_hover(mplayer, button.btn_canvas);
}

bool mplayer_buttonmanager_tbutton_hover(mplayer_t* mplayer, tbtn_t button) {
    return mplayer_rect_hover(mplayer, button.btn_canvas);
}


bool mplayer_buttonmanager_ibuttons_hover(mplayer_t* mplayer, ibtn_t* buttons, int* btn_id, size_t button_count) {
    for(size_t i=0;i<button_count;i++) {
        if(mplayer_buttonmanager_ibutton_hover(mplayer, buttons[i])) {
            if(btn_id != NULL) {
                *btn_id = buttons[i].id;
            }
            buttons[i].hover = true; return true;
        }
    }
    return false;
}

bool mplayer_buttonmanager_tbuttons_hover(mplayer_t* mplayer, tbtn_t* buttons, int* btn_id, size_t button_count) {
    for(size_t i=0;i<button_count;i++) {
        if(mplayer_buttonmanager_tbutton_hover(mplayer, buttons[i])) {
            if(btn_id != NULL) {
                *btn_id = buttons[i].id;
            }
            buttons[i].hover = true; return true;
        }
    }
    return false;
}