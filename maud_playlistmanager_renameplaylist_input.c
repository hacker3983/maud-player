#include "maud_playlistmanager.h"
#include "maud_notification.h"

void maud_playlistmanager_inputbox_init_rename_input(maud_t* maud,
    maud_renameplaylist_input_t* rename_input, const char* input_data) {
    SDL_Rect input_canvas = {.w = 350, .h = 60};
    SDL_Color input_canvascolor = {0},
              cursor_color = {0x00, 0xff, 0x00, 0xff},
              content_color = white;
    rename_input->inputbox = maud_inputbox_create(maud->music_font, 20,
        input_canvascolor, NULL, (SDL_Color){0}, cursor_color, content_color, input_canvas.x, input_canvas.y,
        input_canvas.w, input_canvas.h, 2, 30);
    maud_inputbox_addinputdata(&rename_input->inputbox, input_data);
}

void maud_playlistmanager_inputbox_display_rename_input(maud_t* maud,
    maud_renameplaylist_input_t* rename_input) {
    printf("Rendering rename input...\n");
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistmenu_t* playlist_menu = &playlist_manager->playlist_menu;
    maud_inputbox_t *rename_inputbox = &rename_input->inputbox;
    maud_playlistmenubtn_t* rename_btn = &playlist_menu->renamebtn;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_selectionindex = playlist_manager->playlist_props.selection_index;
    text_info_t rename_text = {
        .font_size = 18,
        .text = "Rename playlist",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Rect playlist_card = {
        .x = 0,
        .y = 0,
        .w = 100,
        .h = 150
    };
    SDL_Color playlist_cardcolor = {0x12, 0x12, 0x12, 0xff};
    SDL_Rect playlist_icon = {
        .x = 0,
        .y = 0,
        .w = 40,
        .h = 40
    };
    maud_textmanager_sizetext(maud->font, &rename_text);
    int max_w = (rename_inputbox->canvas.w > rename_text.text_canvas.w) ?
                    rename_inputbox->canvas.w : rename_text.text_canvas.w;
    text_info_t rename_btntext = {
        .font_size = 18,
        .text = "Rename",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &rename_btntext);
    SDL_Rect rename_btncanvas = {
        .x = 0,
        .y = 0,
        .w = rename_btntext.text_canvas.w + 20,
        .h = rename_btntext.text_canvas.h + 20,
    };
    SDL_Color rename_btncolor = {0x00, 0x00, 0xff, 0xff};
    text_info_t cancel_btntext = {
        .font_size = 18,
        .text = "Cancel",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &cancel_btntext);
    SDL_Rect cancel_btncanvas = {
        .x = 0,
        .y = 0,
        .w = cancel_btntext.text_canvas.w + 20,
        .h = cancel_btntext.text_canvas.h + 20
    };
    SDL_Color cancel_btncolor = {0xff, 0x00, 0x00, 0xff};
    //maud_inputbox_getsize(maud, rename_inputbox);
    SDL_Rect box_container = {
        .x = 0, .y = 0,
        .w = max_w + 60,
        .h = rename_text.text_canvas.h + playlist_card.h + rename_inputbox->canvas.h + rename_btncanvas.h + 80
    };
    SDL_Color box_color = {0xF6, 0xAE, 0x2D, 0xff};
    box_container.x = (maud->win_width - box_container.w) / 2;
    box_container.y = (maud->win_height - box_container.h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(box_color));
    SDL_RenderDrawRect(maud->renderer, &box_container);
    SDL_RenderFillRect(maud->renderer, &box_container);

    rename_text.text_canvas.x = box_container.x + 10;
    rename_text.text_canvas.y = box_container.y + 5;
    SDL_Texture* rename_texture = maud_textmanager_rendertext(maud, maud->font, &rename_text);
    SDL_RenderCopy(maud->renderer, rename_texture, NULL, &rename_text.text_canvas);
    SDL_DestroyTexture(rename_texture);

    playlist_card.x = box_container.x + (box_container.w - playlist_card.w) / 2;
    playlist_card.y = rename_text.text_canvas.y + rename_text.text_canvas.h + 15;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(playlist_cardcolor));
    SDL_RenderDrawRect(maud->renderer, &playlist_card);
    SDL_RenderFillRect(maud->renderer, &playlist_card);

    playlist_icon.x = playlist_card.x + (playlist_card.w - playlist_icon.w) / 2;
    playlist_icon.y = playlist_card.y + (playlist_card.h - playlist_icon.h) / 2;
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE][music_playlistbtn.texture_idx],
        NULL, &playlist_icon);

    rename_inputbox->canvas.x = box_container.x + (box_container.w - rename_inputbox->canvas.w) / 2,
    rename_inputbox->canvas.y = playlist_card.y + playlist_card.h + 15;

    SDL_Rect* placeholder_canvas = &rename_inputbox->placeholder_canvas;
    placeholder_canvas->x = rename_inputbox->canvas.x + 10;
    placeholder_canvas->y = rename_inputbox->canvas.y + (rename_inputbox->canvas.h
        - placeholder_canvas->h) / 2;
    maud_inputbox_display(maud, rename_inputbox);

    rename_btncanvas.x = box_container.x + 10;
    rename_btncanvas.y = box_container.y + box_container.h - rename_btncanvas.h - 20;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(rename_btncolor));
    SDL_RenderDrawRect(maud->renderer, &rename_btncanvas);
    SDL_RenderFillRect(maud->renderer, &rename_btncanvas);

    rename_btntext.text_canvas.x = rename_btncanvas.x + (rename_btncanvas.w - rename_btntext.text_canvas.w) / 2;
    rename_btntext.text_canvas.y = rename_btncanvas.y + (rename_btncanvas.h - rename_btntext.text_canvas.h) / 2;
    SDL_Texture* rename_btntexture = maud_textmanager_rendertext(maud, maud->font, &rename_btntext);
    SDL_RenderCopy(maud->renderer, rename_btntexture, NULL, &rename_btntext.text_canvas);

    cancel_btncanvas.x = box_container.x + box_container.w - cancel_btncanvas.w - 10;
    cancel_btncanvas.y = rename_btncanvas.y;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(cancel_btncolor));
    SDL_RenderDrawRect(maud->renderer, &cancel_btncanvas);
    SDL_RenderFillRect(maud->renderer, &cancel_btncanvas);

    cancel_btntext.text_canvas.x = cancel_btncanvas.x + (cancel_btncanvas.w - cancel_btntext.text_canvas.w) / 2;
    cancel_btntext.text_canvas.y = cancel_btncanvas.y + (cancel_btncanvas.h - cancel_btntext.text_canvas.h) / 2;
    SDL_Texture* cancel_btntexture = maud_textmanager_rendertext(maud, maud->font, &cancel_btntext);
    SDL_RenderCopy(maud->renderer, cancel_btntexture, NULL, &cancel_btntext.text_canvas);
    SDL_DestroyTexture(cancel_btntexture);

    if(maud->mouse_clicked && !maud_rect_hover(maud, box_container)) {
        maud_inputbox_clear(&rename_input->inputbox);
        rename_btn->clicked = false;
        maud->mouse_clicked = false;
    } else if(maud_rect_hover(maud, rename_btncanvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            size_t new_namelen = 0;
            char* new_name = rename_inputbox->input.data;
            char* original_name = playlists[playlist_selectionindex].name;
            size_t msg_len = 35 + strlen(original_name) + strlen(new_name);
            char msg_buff[msg_len+1];
            sprintf(msg_buff, "Playlist '%s' has been renamed to '%s'.", original_name, new_name);
            msg_buff[msg_len] = '\0';
            maud_playlist_rename(&playlist_manager->playlists, playlist_manager->playlist_count, original_name,
                new_name);
            maud_playlistmanager_write_data_tofile(maud);
            maud_inputbox_clear(&rename_input->inputbox);
            maud_notification_push(&maud->notification, maud->font, 20,
                (SDL_Color){0x12, 0x12, 0x12, 0xff},
                msg_buff,
                (SDL_Color){0x00, 0xff, 0x00, 0xff},
                1.5,
                20,
                20,
                10
            );
            rename_btn -> clicked = false;
            maud->mouse_clicked = false;
        }
    } else if(maud_rect_hover(maud, cancel_btncanvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud_inputbox_clear(&rename_input->inputbox);
            rename_btn->clicked = false;
            maud->mouse_clicked = false;
        }
    } else if(maud->mouse_clicked) {
        maud->mouse_clicked = false;
    }
}