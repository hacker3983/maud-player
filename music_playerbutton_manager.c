#include "music_playerbutton_manager.h"

bool mplayer_buttonmanager_ibutton_hover(mplayer_t* mplayer, ibtn_t button) {
    int x = button.btn_canvas.x, y = button.btn_canvas.y,
        w = button.btn_canvas.w, h = button.btn_canvas.h;
    if((mplayer->mouse_x <= x + h && mplayer->mouse_x >= x) &&
        (mplayer->mouse_y <= y + h && mplayer->mouse_y >= y)) {
            return true;
    }
    return false;
}

bool mplayer_buttonmanager_tbutton_hover(mplayer_t* mplayer, tbtn_t button) {
    int x = button.btn_canvas.x, y = button.btn_canvas.y,
        w = button.btn_canvas.w, h = button.btn_canvas.h;
    if((mplayer->e.motion.x <= x + h && mplayer->e.motion.x >= x) &&
        (mplayer->e.motion.y <= y + h && mplayer->e.motion.y >= y)) {
            return true;
    }
    return false;
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