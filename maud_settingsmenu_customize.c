#include "maud_settingsmenu.h"
#include "maud_textmanager.h"
#include "maud_colorpicker.h"

void maud_settingmenu_rendercustomizetab_headertext(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    int start_x = navbar->canvas.x + navbar->canvas.w + 10;
    SDL_Rect backbtn_canvas = {
        .w = 50, .h = 50,
        .x = start_x,
    };
    if(customize_tab->show_colorpicker) {
        if(maud_rect_hover(maud, backbtn_canvas)) {
            maud_setcursor(maud, MAUD_CURSOR_POINTER);
            if(maud->mouse_clicked) {
                customize_tab->show_colorpicker = false;
                maud->mouse_clicked = false;
            }
        }
        SDL_Texture* backbtn_texture = IMG_LoadTexture(maud->renderer, "images/back-button.png");
        SDL_RenderCopy(maud->renderer, backbtn_texture, NULL, &backbtn_canvas);
        SDL_DestroyTexture(backbtn_texture);
        start_x += backbtn_canvas.w;
    }
    text_info_t header_text = {
        .font_size = 36,
        .text = "Customize",
        .text_canvas = {
            .x = start_x + 10
        },
        .text_color = {0xFF, 0xFF, 0x00, 0xFF},
    };
    maud_textmanager_sizetext(maud->font, &header_text);
    SDL_Texture* customize_texture = maud_textmanager_rendertext(maud, maud->font,
        &header_text);
    SDL_RenderCopy(maud->renderer, customize_texture, NULL,
        &header_text.text_canvas);
    SDL_DestroyTexture(customize_texture);
    customize_tab->header_text = header_text;
}

void maud_settingmenu_rendercustomizetab_colorpicker(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    if(!customize_tab->show_colorpicker) {
        return;
    }
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t header_text = customize_tab->header_text;
    maud_colorpicker_t* color_picker = &customize_tab->color_picker;
    maud_colorpicker_setposition(color_picker,
        navbar->canvas.x + navbar->canvas.w + 10,
        header_text.text_canvas.y + header_text.text_canvas.h + 20
    );
    maud_colorpicker_display(maud, color_picker);
}

void maud_settingmenu_rendercustomizetab_musictile_modifier(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    if(customize_tab->show_colorpicker) {
        return;
    }
    text_info_t header_text = customize_tab->header_text;
    maud_musictile_modifier_t musictile_modifier = {
        .backgroundcolor_content = {    
            .font_size = 30,
            .text = "Music Tile Background Color: ",
            .text_color = white,
            .text_canvas = {
                .x = header_text.text_canvas.x,
                .y = header_text.text_canvas.y + header_text.text_canvas.h + 20
            }
        },
        .bgcolor_button = {
            .text = {
                .font_size = 20,
                .text = "Change Background Color",
                .text_color = white
            },
            .color = {0xFF, 0x00, 0x00, 0xFF}
        },
        .textcolor_content = {
            .font_size = 30,
            .text = "Music Tile Text Color: ",
            .text_color = white,
            .text_canvas = {
                .x = header_text.text_canvas.x
            }
        },
        .textcolor_button = {
            .text = {
                .font_size = 20,
                .text = "Change Text Color",
                .text_color = white
            },
            .color = {0xFF, 0x00, 0x00, 0xFF}
        }
    };
    text_info_t *backgroundcolor_content = &musictile_modifier.backgroundcolor_content,
                *textcolor_content = &musictile_modifier.textcolor_content;
    maud_textmanager_sizetext(maud->font, backgroundcolor_content);
    SDL_Texture* bgcolor_texture = maud_textmanager_rendertext(maud, maud->font, backgroundcolor_content);
    SDL_RenderCopy(maud->renderer, bgcolor_texture, NULL, &backgroundcolor_content->text_canvas);
    SDL_DestroyTexture(bgcolor_texture);

    maud_customizetab_btn_t* bgcolor_btn = &musictile_modifier.bgcolor_button;
    maud_textmanager_sizetext(maud->font, &bgcolor_btn->text);
    bgcolor_btn->canvas.x = backgroundcolor_content->text_canvas.x +
        backgroundcolor_content->text_canvas.w;
    bgcolor_btn->canvas.w = bgcolor_btn->text.text_canvas.w + 20,
    bgcolor_btn->canvas.h = bgcolor_btn->text.text_canvas.h + 20;
    bgcolor_btn->canvas.y = backgroundcolor_content->text_canvas.y +
        (backgroundcolor_content->text_canvas.h - bgcolor_btn->canvas.h) / 2;
    if(maud_rect_hover(maud, bgcolor_btn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            customize_tab->customize_id = CUSTOMIZE_MUSICTILE_BACKGROUNDCOLOR;
            customize_tab->show_colorpicker = true;
            maud->mouse_clicked = false;
        }
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(bgcolor_btn->color));
    SDL_RenderDrawRect(maud->renderer, &bgcolor_btn->canvas);
    SDL_RenderFillRect(maud->renderer, &bgcolor_btn->canvas);
    text_info_t* text_ptr = &bgcolor_btn->text;
    text_ptr->text_canvas.x = bgcolor_btn->canvas.x + (bgcolor_btn->canvas.w -
        text_ptr->text_canvas.w) / 2;
    text_ptr->text_canvas.y = bgcolor_btn->canvas.y + (bgcolor_btn->canvas.h -
        text_ptr->text_canvas.h) / 2;
    SDL_Texture* btn_texture1 = maud_textmanager_rendertext(maud, maud->font, text_ptr);
    SDL_RenderCopy(maud->renderer, btn_texture1, NULL, &text_ptr->text_canvas);
    SDL_DestroyTexture(btn_texture1);

    maud_textmanager_sizetext(maud->font, textcolor_content);
    textcolor_content->text_canvas.x = backgroundcolor_content->text_canvas.x;
    textcolor_content->text_canvas.y = bgcolor_btn->canvas.y +
        bgcolor_btn->canvas.h + 30;
    SDL_Texture* textcolor_texture = maud_textmanager_rendertext(maud, maud->font, textcolor_content);
    SDL_RenderCopy(maud->renderer, textcolor_texture, NULL, &textcolor_content->text_canvas);
    SDL_DestroyTexture(textcolor_texture);

    maud_customizetab_btn_t* textcolor_btn = &musictile_modifier.textcolor_button;
    maud_textmanager_sizetext(maud->font, &textcolor_btn->text);
    textcolor_btn->canvas.x = textcolor_content->text_canvas.x +
        textcolor_content->text_canvas.w;
    textcolor_btn->canvas.w = textcolor_btn->text.text_canvas.w + 20,
    textcolor_btn->canvas.h = textcolor_btn->text.text_canvas.h + 20;
    textcolor_btn->canvas.y = textcolor_content->text_canvas.y +
        (textcolor_content->text_canvas.h - textcolor_btn->canvas.h) / 2;
    if(maud_rect_hover(maud, textcolor_btn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            customize_tab->customize_id = CUSTOMIZE_MUSICTILE_TEXTCOLOR;
            customize_tab->show_colorpicker = true;
            maud->mouse_clicked = false;
        }
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(textcolor_btn->color));
    SDL_RenderDrawRect(maud->renderer, &textcolor_btn->canvas);
    SDL_RenderFillRect(maud->renderer, &textcolor_btn->canvas);

    text_ptr = &textcolor_btn->text;
    text_ptr->text_canvas.x = textcolor_btn->canvas.x + (textcolor_btn->canvas.w -
        text_ptr->text_canvas.w) / 2;
    text_ptr->text_canvas.y = textcolor_btn->canvas.y + (textcolor_btn->canvas.h -
        text_ptr->text_canvas.h) / 2;
    SDL_Texture* btn_texture2 = maud_textmanager_rendertext(maud, maud->font, text_ptr);
    SDL_RenderCopy(maud->renderer, btn_texture2, NULL, &text_ptr->text_canvas);
    SDL_DestroyTexture(btn_texture2);
}

void maud_settingmenu_displaycustomize_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_customizetab_t* customize_tab = &navbar->customize_tab;
    maud_settingmenu_rendercustomizetab_headertext(maud, customize_tab);
    maud_settingmenu_rendercustomizetab_musictile_modifier(maud, customize_tab);
    maud_settingmenu_rendercustomizetab_colorpicker(maud, customize_tab);
}