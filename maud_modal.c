#include "maud_modal.h"
#include "maud_textmanager.h"
#include "maud_inputboxes.h"
#include "maud_playlistmanager.h"

void maud_modal_getsize(maud_t* maud, maud_modal_t* modal) {
    maud_modal_header_t* header = &modal->header;
    text_info_t* title = &header->title;
    maud_textmanager_sizetext(modal->font, title);

    header->canvas.w = modal->canvas.w,
    header->canvas.h = title->text_canvas.h + header->padding_height;

    maud_modal_button_t* confirm_button = &modal->confirm_button;
    text_info_t* confirm_text = &confirm_button->text;
    maud_textmanager_sizetext(modal->font, confirm_text);

    confirm_button->canvas.w = confirm_text->text_canvas.w + confirm_button->padding_width;
    confirm_button->canvas.h = confirm_text->text_canvas.h + confirm_button->padding_height;

    maud_modal_button_t* cancel_button = &modal->cancel_button;
    text_info_t* cancel_text = &cancel_button->text;
    maud_textmanager_sizetext(modal->font, cancel_text);

    cancel_button->canvas.w = cancel_text->text_canvas.w + cancel_button->padding_width;
    cancel_button->canvas.h = cancel_text->text_canvas.h + cancel_button->padding_height;

    maud_modal_buttoncontainer_t* button_container = &modal->button_container;
    button_container->canvas.w = modal->canvas.w;
    int max_btnh = maud_playlistmanager_findmaxheight((SDL_Rect[]){
        confirm_button->canvas,
        cancel_button->canvas
    }, 2);
    button_container->canvas.h = max_btnh + button_container->padding_height;
}

void maud_modal_setposition(maud_t* maud, maud_modal_t* modal) {
    modal->canvas.x = (maud->win_width - modal->canvas.w) / 2;
    modal->canvas.y = (maud->win_height - modal->canvas.h) / 2;

    maud_modal_header_t* header = &modal->header;
    text_info_t* title = &header->title;
    title->text_canvas.x = modal->canvas.x + header->padding_x,
    title->text_canvas.y = modal->canvas.y + header->padding_y;

    header->canvas.x = modal->canvas.x,
    header->canvas.y = modal->canvas.y;

    maud_modal_image_t* image = &modal->image;
    image->canvas.x = modal->canvas.x + (modal->canvas.w - image->canvas.w) / 2;
    image->canvas.y = header->canvas.y + header->canvas.h + image->padding_y;

    maud_modal_input_t* input = &modal->input;
    maud_inputbox_t* inputbox = &input->inputbox;
    inputbox->canvas.x = modal->canvas.x + (modal->canvas.w - inputbox->canvas.w) / 2;
    inputbox->canvas.y = image->canvas.y + image->canvas.h + input->padding_y;

    SDL_Rect *placeholder_canvas = &inputbox->placeholder_canvas;
    placeholder_canvas->x = inputbox->canvas.x + 10;
    placeholder_canvas->y = inputbox->canvas.y + (inputbox->canvas.h -
        placeholder_canvas->h) / 2;

    maud_modal_buttoncontainer_t* button_container = &modal->button_container;
    button_container->canvas.x = modal->canvas.x;
    button_container->canvas.y = modal->canvas.y + modal->canvas.h - button_container->canvas.h;

    maud_modal_button_t* confirm_button = &modal->confirm_button;
    confirm_button->canvas.x = modal->canvas.x + confirm_button->padding_x;
    confirm_button->canvas.y = modal->canvas.y + modal->canvas.h - confirm_button->canvas.h
        - confirm_button->padding_y;
    if(modal->enable_buttoncontainer) {
        confirm_button->canvas.y = button_container->canvas.y + (button_container->canvas.h -
            confirm_button->canvas.h) / 2;
    }

    text_info_t* confirm_text = &confirm_button->text;
    confirm_text->text_canvas.x = confirm_button->canvas.x + (confirm_button->canvas.w -
        confirm_text->text_canvas.w) / 2;
    confirm_text->text_canvas.y = confirm_button->canvas.y + (confirm_button->canvas.h -
        confirm_text->text_canvas.h) / 2;

    maud_modal_button_t* cancel_button = &modal->cancel_button;
    cancel_button->canvas.x = modal->canvas.x + modal->canvas.w - cancel_button->canvas.w
        - cancel_button->padding_x;
    cancel_button->canvas.y = modal->canvas.y + modal->canvas.h - cancel_button->canvas.h
        - cancel_button->padding_y;
    if(modal->enable_buttoncontainer) {
        cancel_button->canvas.y = button_container->canvas.y + (button_container->canvas.h -
            cancel_button->canvas.h) / 2;
    }

    text_info_t* cancel_text = &cancel_button->text;
    cancel_text->text_canvas.x = cancel_button->canvas.x + (cancel_button->canvas.w -
        cancel_text->text_canvas.w) / 2;
    cancel_text->text_canvas.y = cancel_button->canvas.y + (cancel_button->canvas.h -
        cancel_text->text_canvas.h) / 2;
}

void maud_modal_display(maud_t* maud, maud_modal_t* modal) {
    maud_modal_getsize(maud, modal);
    maud_modal_setposition(maud, modal);
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(modal->color));
    SDL_RenderDrawRect(maud->renderer, &modal->canvas);
    SDL_RenderFillRect(maud->renderer, &modal->canvas);

    maud_modal_header_t* header = &modal->header;
    if(header->show_bgcolor) {
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(header->color));
        SDL_RenderDrawRect(maud->renderer, &header->canvas);
        SDL_RenderFillRect(maud->renderer, &header->canvas);
    }

    text_info_t* title = &header->title;
    SDL_Texture* title_texture = maud_textmanager_rendertext(maud, modal->font, title);
    SDL_RenderCopy(maud->renderer, title_texture, NULL, &title->text_canvas);

    maud_modal_image_t* image = &modal->image;
    if(image->show_bgcolor) {
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(image->color));
        SDL_RenderDrawRect(maud->renderer, &image->canvas);
        SDL_RenderFillRect(maud->renderer, &image->canvas);
    }
    if(!image->texture && image->path) {
        image->texture = IMG_LoadTexture(maud->renderer, image->path);
    }
    if(image->texture) {
        SDL_RenderCopy(maud->renderer, image->texture, NULL, &image->canvas);
    }

    maud_modal_input_t* input = &modal->input;
    maud_inputbox_t* inputbox = &input->inputbox;
    maud_inputbox_display(maud, inputbox);

    maud_modal_buttoncontainer_t* button_container = &modal->button_container;
    if(modal->enable_buttoncontainer) {
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(button_container->color));
        SDL_RenderDrawRect(maud->renderer, &button_container->canvas);
        SDL_RenderFillRect(maud->renderer, &button_container->canvas);
    }

    maud_modal_button_t* confirm_button = &modal->confirm_button;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(confirm_button->color));
    SDL_RenderDrawRect(maud->renderer, &confirm_button->canvas);
    SDL_RenderFillRect(maud->renderer, &confirm_button->canvas);

    SDL_Texture* confirmtext_texture = maud_textmanager_rendertext(maud, modal->font, &confirm_button->text);
    SDL_RenderCopy(maud->renderer, confirmtext_texture, NULL, &confirm_button->text.text_canvas);
    SDL_DestroyTexture(confirmtext_texture);

    maud_modal_button_t* cancel_button = &modal->cancel_button;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(cancel_button->color));
    SDL_RenderDrawRect(maud->renderer, &cancel_button->canvas);
    SDL_RenderFillRect(maud->renderer, &cancel_button->canvas);

    SDL_Texture* canceltext_texture = maud_textmanager_rendertext(maud, modal->font, &cancel_button->text);
    SDL_RenderCopy(maud->renderer, canceltext_texture, NULL, &cancel_button->text.text_canvas);
    SDL_DestroyTexture(canceltext_texture);
}

void maud_modal_handle_inputevents(maud_t* maud, maud_modal_t* modal) {
    maud_modal_input_t* input = &modal->input;
    maud_inputbox_t* inputbox = &input->inputbox;
    maud_inputbox_handle_events(maud, inputbox);
}

void maud_modal_handle_events(maud_t* maud, maud_modal_t* modal) {
    maud_modal_button_t *confirm_button = &modal->confirm_button,
                        *cancel_button = &modal->cancel_button;
    maud_modal_input_t* input = &modal->input;
    maud_inputbox_t* inputbox = &input->inputbox;
    if(!maud_rect_hover(maud, modal->canvas) && maud->mouse_clicked) {
        maud_inputbox_clear(inputbox);
        modal->leave = true;
        return;
    }
    if(maud_rect_hover(maud, confirm_button->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            confirm_button->clicked = true;
            maud->mouse_clicked = false;
        }
    } else if(maud_rect_hover(maud, cancel_button->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud_inputbox_clear(inputbox);
            cancel_button->clicked = true;
            maud->mouse_clicked = false;
        }
    }
}

void maud_modal_destroy(maud_modal_t* modal) {
    maud_modal_image_t* image = &modal->image;
    if(image->path) {
        free(image->path);
        SDL_DestroyTexture(image->texture);
    }
}