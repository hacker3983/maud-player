#include "maud_settingsmenu.h"
#include "maud_textmanager.h"
#include "maud_colorpicker.h"
#include "maud_notification.h"

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
    maud_iteminfo_t* item_info = &maud->item_info;
    maud_tileinfo_t* tile_info = &item_info->tile_info;
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t header_text = customize_tab->header_text;
    maud_colorpicker_t* color_picker = &customize_tab->color_picker;
    maud_colorpicker_setposition(color_picker,
        navbar->canvas.x + navbar->canvas.w + 10,
        header_text.text_canvas.y + header_text.text_canvas.h + 20
    );
    maud_colorpicker_display(maud, color_picker);

    maud_customizetab_btn_t save_btn = {
        .text = {
            .font_size = 20,
            .text = "Save Changes",
            .text_color = white
        },
        .color = {0xff, 0x00, 0x00, 0xff}
    };
    maud_textmanager_sizetext(maud->font, &save_btn.text);
    SDL_Rect *canvas = &save_btn.canvas,
             *text_canvas = &save_btn.text.text_canvas;
    canvas->w = text_canvas->w + 10,
    canvas->h = text_canvas->h + 10;
    canvas->x = (maud->win_width - canvas->w) / 2;
    canvas->y = color_picker->canvas.y + color_picker->canvas.h;
    text_canvas->x = canvas->x + (canvas->w - text_canvas->w) / 2;
    text_canvas->y = canvas->y + (canvas->h - text_canvas->h) / 2;

    if(maud_rect_hover(maud, save_btn.canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            switch(customize_tab->customize_id) {
                case CUSTOMIZE_MUSICTILE_BACKGROUNDCOLOR:
                    maud_tileinfo_update_backgroundcolor(maud, color_picker->color);
                    break;
                case CUSTOMIZE_MUSICTILE_TEXTCOLOR:
                    maud_tileinfo_update_foregroundcolor(maud, color_picker->color);
                    tile_info->updated = true;
                    break;
                case CUSTOMIZE_STATUSBAR_BACKGROUNDCOLOR:
                    maud_statusinfo_update_backgroundcolor(maud, color_picker->color);
                    break;
                case CUSTOMIZE_STATUSBAR_PROGRESSBAR_COLOR:
                    maud_statusinfo_update_progressbar_color(maud, color_picker->color);
                    break;
                case CUSTOMIZE_STATUSBAR_PROGRESSLINE_COLOR:
                    maud_statusinfo_update_progressline_color(maud, color_picker->color);
                    break;
            }
            maud_iteminfo_write_data(maud);
            printf("Successfully updated music tile color\n");
            maud->mouse_clicked = false;
        }
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(save_btn.color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);
    SDL_Texture* text_texture = maud_textmanager_rendertext(maud, maud->font,
        &save_btn.text);
    SDL_RenderCopy(maud->renderer, text_texture, NULL, text_canvas);
    SDL_DestroyTexture(text_texture);
    customize_tab->save_btn = save_btn;
}

void maud_settingmenu_rendercustomizetab_musictile_modifier(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    maud_colorpicker_t* color_picker = &customize_tab->color_picker;
    maud_iteminfo_t* item_info = &maud->item_info;
    maud_tileinfo_t* tile_info = &item_info->tile_info;
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
            maud_colorpicker_setcolor(color_picker,
                tile_info->background_color.r,
                tile_info->background_color.g,
                tile_info->background_color.b,
                tile_info->background_color.a
            );
            maud_colorpicker_setcolorfromhandle(color_picker);
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
            maud_colorpicker_setcolor(color_picker,
                tile_info->foreground_color.r,
                tile_info->foreground_color.g,
                tile_info->foreground_color.b,
                tile_info->foreground_color.a
            );
            maud_colorpicker_setcolorfromhandle(color_picker);
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
    customize_tab->musictile_modifier = musictile_modifier;
}

void maud_settingmenu_rendercustomizetab_statusbar_modifier(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    maud_colorpicker_t* color_picker = &customize_tab->color_picker;
    maud_iteminfo_t* item_info = &maud->item_info;
    maud_statusbarinfo_t* statusbar_info = &item_info->statusbar_info;
    if(customize_tab->show_colorpicker) {
        return;
    }
    maud_musictile_modifier_t* musictile_modifier = &customize_tab->musictile_modifier;
    maud_customizetab_btn_t musictile_textcolorbtn = musictile_modifier->textcolor_button;
    SDL_Rect musictile_btncanvas = musictile_textcolorbtn.canvas;
    text_info_t header_text = customize_tab->header_text;
    maud_statusbar_modifier_t statusbar_modifier = {
        .bgcolor_content = {
            .font_size = 30,
            .text = "Music Status Bar Color: ",
            .text_color = white,
            .text_canvas = {
                .x = header_text.text_canvas.x,
                .y = musictile_btncanvas.y + musictile_btncanvas.h + 30
            }
        },
        .bgcolor_btn = {
            .text = {
                .font_size = 20,
                .text = "Change Color",
                .text_color = white
            },
            .color = {0xFF, 0x00, 0x00, 0xFF}
        },
        .progressbar_colorcontent = {
            .font_size = 30,
            .text = "Progress Bar Color: ",
            .text_color = white
        },
        .progressbar_colorbtn = {
            .color = {0xFF, 0x00, 0x00, 0xFF},
            .text = {
                .font_size = 20,
                .text = "Change Color",
                .text_color = white
            }
        },
        .progressline_colorcontent = {
            .font_size = 30,
            .text = "Progress Line Color: ",
            .text_color = white
        },
        .progressline_colorbtn = {
            .color = {0xFF, 0x00, 0x00, 0xFF},
            .text = {
                .font_size = 20,
                .text = "Change Color",
                .text_color = white
            }
        }
    };
    text_info_t *bgcolor_content = &statusbar_modifier.bgcolor_content,
                *progressbar_content = &statusbar_modifier.progressbar_colorcontent,
                *progressline_content = &statusbar_modifier.progressline_colorcontent;
    maud_textmanager_sizetext(maud->font, bgcolor_content);
    SDL_Texture* bgcolor_texture = maud_textmanager_rendertext(maud, maud->font,
        bgcolor_content);
    SDL_RenderCopy(maud->renderer, bgcolor_texture, NULL, &bgcolor_content->text_canvas);
    SDL_DestroyTexture(bgcolor_texture);

    maud_customizetab_btn_t* music_statusbtn = &statusbar_modifier.bgcolor_btn;
    maud_textmanager_sizetext(maud->font, &music_statusbtn->text);
    music_statusbtn->canvas.x = bgcolor_content->text_canvas.x +
        bgcolor_content->text_canvas.w;
    music_statusbtn->canvas.w = music_statusbtn->text.text_canvas.w +
        20;
    music_statusbtn->canvas.h = music_statusbtn->text.text_canvas.h +
        20;
    music_statusbtn->canvas.y = bgcolor_content->text_canvas.y + (
        bgcolor_content->text_canvas.h - music_statusbtn->canvas.h) / 2;
    if(maud_rect_hover(maud, music_statusbtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud_colorpicker_setcolor(color_picker,
                statusbar_info->color.r,
                statusbar_info->color.g,
                statusbar_info->color.b,
                statusbar_info->color.a
            );
            maud_colorpicker_setcolorfromhandle(color_picker);
            customize_tab->customize_id = CUSTOMIZE_STATUSBAR_BACKGROUNDCOLOR;
            customize_tab->show_colorpicker = true;
            maud->mouse_clicked = false;
        }
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(music_statusbtn->color));
    SDL_RenderDrawRect(maud->renderer, &music_statusbtn->canvas);
    SDL_RenderFillRect(maud->renderer, &music_statusbtn->canvas);

    text_info_t* text_ptr = &music_statusbtn->text;
    text_ptr->text_canvas.x = music_statusbtn->canvas.x + (music_statusbtn->canvas.w -
        text_ptr->text_canvas.w) / 2;
    text_ptr->text_canvas.y = music_statusbtn->canvas.y + (music_statusbtn->canvas.h -
        text_ptr->text_canvas.h) / 2;
    SDL_Texture* btn_texture1 = maud_textmanager_rendertext(maud, maud->font, text_ptr);
    SDL_RenderCopy(maud->renderer, btn_texture1, NULL, &text_ptr->text_canvas);
    SDL_DestroyTexture(btn_texture1);

    maud_textmanager_sizetext(maud->font, progressbar_content);
    SDL_Texture* progressbar_texture = maud_textmanager_rendertext(maud, maud->font,
        progressbar_content);
    progressbar_content->text_canvas.x = bgcolor_content->text_canvas.x;
    progressbar_content->text_canvas.y = music_statusbtn->canvas.y +
        music_statusbtn->canvas.h + 30;
    SDL_RenderCopy(maud->renderer, progressbar_texture, NULL, &progressbar_content->text_canvas);
    SDL_DestroyTexture(progressbar_texture);

    maud_customizetab_btn_t* progressbar_btn = &statusbar_modifier.progressbar_colorbtn;
    maud_textmanager_sizetext(maud->font, &progressbar_btn->text);
    progressbar_btn->canvas.x = progressbar_content->text_canvas.x +
        progressbar_content->text_canvas.w;
    progressbar_btn->canvas.w = progressbar_btn->text.text_canvas.w +
        20;
    progressbar_btn->canvas.h = progressbar_btn->text.text_canvas.h +
        20;
    progressbar_btn->canvas.y = progressbar_content->text_canvas.y + (
        progressbar_content->text_canvas.h - progressbar_btn->canvas.h) / 2;
    if(maud_rect_hover(maud, progressbar_btn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud_colorpicker_setcolor(color_picker,
                statusbar_info->progressbar_color.r,
                statusbar_info->progressbar_color.g,
                statusbar_info->progressbar_color.b,
                statusbar_info->progressbar_color.a
            );
            maud_colorpicker_setcolorfromhandle(color_picker);
            customize_tab->customize_id = CUSTOMIZE_STATUSBAR_PROGRESSBAR_COLOR;
            customize_tab->show_colorpicker = true;
            maud->mouse_clicked = false;
        }
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(progressbar_btn->color));
    SDL_RenderDrawRect(maud->renderer, &progressbar_btn->canvas);
    SDL_RenderFillRect(maud->renderer, &progressbar_btn->canvas);

    text_ptr = &progressbar_btn->text;
    text_ptr->text_canvas.x = progressbar_btn->canvas.x + (progressbar_btn->canvas.w -
        text_ptr->text_canvas.w) / 2;
    text_ptr->text_canvas.y = progressbar_btn->canvas.y + (progressbar_btn->canvas.h -
        text_ptr->text_canvas.h) / 2;
    SDL_Texture* btn_texture2 = maud_textmanager_rendertext(maud, maud->font, text_ptr);
    SDL_RenderCopy(maud->renderer, btn_texture2, NULL, &text_ptr->text_canvas);
    SDL_DestroyTexture(btn_texture2);

    maud_textmanager_sizetext(maud->font, progressline_content);
    SDL_Texture* progressline_texture = maud_textmanager_rendertext(maud, maud->font,
        progressline_content);
    progressline_content->text_canvas.x = progressbar_content->text_canvas.x;
    progressline_content->text_canvas.y = progressbar_btn->canvas.y +
        progressbar_btn->canvas.h + 30;
    SDL_RenderCopy(maud->renderer, progressline_texture, NULL, &progressline_content->text_canvas);
    SDL_DestroyTexture(progressline_texture);

    maud_customizetab_btn_t* progressline_btn = &statusbar_modifier.progressline_colorbtn;
    maud_textmanager_sizetext(maud->font, &progressline_btn->text);
    progressline_btn->canvas.x = progressline_content->text_canvas.x +
        progressline_content->text_canvas.w;
    progressline_btn->canvas.w = progressline_btn->text.text_canvas.w +
        20;
    progressline_btn->canvas.h = progressline_btn->text.text_canvas.h +
        20;
    progressline_btn->canvas.y = progressline_content->text_canvas.y + (
    progressline_content->text_canvas.h - progressline_btn->canvas.h) / 2;
    if(maud_rect_hover(maud, progressline_btn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud_colorpicker_setcolor(color_picker,
                statusbar_info->progressline_color.r,
                statusbar_info->progressline_color.g,
                statusbar_info->progressline_color.b,
                statusbar_info->progressline_color.a
            );
            maud_colorpicker_setcolorfromhandle(color_picker);
            customize_tab->customize_id = CUSTOMIZE_STATUSBAR_PROGRESSLINE_COLOR;
            customize_tab->show_colorpicker = true;
            maud->mouse_clicked = false;
        }
    }
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(progressline_btn->color));
    SDL_RenderDrawRect(maud->renderer, &progressline_btn->canvas);
    SDL_RenderFillRect(maud->renderer, &progressline_btn->canvas);

    text_ptr = &progressline_btn->text;
    text_ptr->text_canvas.x = progressline_btn->canvas.x + (progressline_btn->canvas.w -
        text_ptr->text_canvas.w) / 2;
    text_ptr->text_canvas.y = progressline_btn->canvas.y + (progressline_btn->canvas.h -
        text_ptr->text_canvas.h) / 2;
    SDL_Texture* btn_texture3 = maud_textmanager_rendertext(maud, maud->font, text_ptr);
    SDL_RenderCopy(maud->renderer, btn_texture3, NULL, &text_ptr->text_canvas);
    SDL_DestroyTexture(btn_texture3);
}

void maud_settingmenu_displaycustomize_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_customizetab_t* customize_tab = &navbar->customize_tab;
    maud_settingmenu_rendercustomizetab_headertext(maud, customize_tab);
    maud_settingmenu_rendercustomizetab_musictile_modifier(maud, customize_tab);
    maud_settingmenu_rendercustomizetab_statusbar_modifier(maud, customize_tab);
    maud_settingmenu_rendercustomizetab_colorpicker(maud, customize_tab);
    maud_notification_display(maud, &maud->notification);
}