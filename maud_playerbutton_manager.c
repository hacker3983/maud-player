#include "maud_playerbutton_manager.h"

bool maud_buttonmanager_ibutton_hover(maud_t* maud, ibtn_t button) {
    return maud_rect_hover(maud, button.btn_canvas);
}

bool maud_buttonmanager_tbutton_hover(maud_t* maud, tbtn_t button) {
    return maud_rect_hover(maud, button.btn_canvas);
}


bool maud_buttonmanager_ibuttons_hover(maud_t* maud, ibtn_t* buttons, int* btn_id, size_t button_count) {
    for(size_t i=0;i<button_count;i++) {
        if(maud_buttonmanager_ibutton_hover(maud, buttons[i])) {
            if(btn_id != NULL) {
                *btn_id = buttons[i].id;
            }
            buttons[i].hover = true; return true;
        }
    }
    return false;
}

bool maud_buttonmanager_tbuttons_hover(maud_t* maud, tbtn_t* buttons, int* btn_id, size_t button_count) {
    for(size_t i=0;i<button_count;i++) {
        if(maud_buttonmanager_tbutton_hover(maud, buttons[i])) {
            if(btn_id != NULL) {
                *btn_id = buttons[i].id;
            }
            buttons[i].hover = true; return true;
        }
    }
    return false;
}