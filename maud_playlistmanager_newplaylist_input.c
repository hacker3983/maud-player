#include "maud_playlistmanager.h"

void maud_playlistmanager_newplaylist_input_init(maud_t* maud,
    maud_newplaylist_input_t* newplaylist_input) {
    if(newplaylist_input->init) {
        return;
    }
    maud_newplaylist_input_t new_input = {
        .canvas = {
            .w = 100, .h = 110
        },
        .color = dark_purple,
        .create_playlistbtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .text = {
                .font_size = 20,
                .text = "Create playlist",
                .text_color = white,
            },
            .color = {0x04, 0x8B, 0xA8, 0xff}
        }
    };
    
    SDL_Rect input_canvas = {.w = 350, .h = 60};
    SDL_Color input_canvascolor = {0};
    char* placeholder_text = "New playlist name here...";
    SDL_Color placeholder_color = white,
              cursor_color = {0x00, 0xff, 0x00, 0xff};
    new_input.inputbox = maud_inputbox_create(maud->music_font, 20, input_canvascolor,
        placeholder_text, placeholder_color, cursor_color, placeholder_color,
        input_canvas.x, input_canvas.y, input_canvas.w, input_canvas.h,
        2, 25
    );
    
    maud_createplaylistbtn_t* createbtn = &new_input.create_playlistbtn;
    maud_textmanager_sizetext(maud->font, &createbtn->text);
    createbtn->canvas.w += createbtn->text.text_canvas.w + 10;
    createbtn->canvas.h += createbtn->text.text_canvas.h + 10;

    SDL_Rect* box_container = &new_input.canvas;
    if(new_input.inputbox.canvas.w > createbtn->canvas.w) {
        box_container->w += new_input.inputbox.canvas.w;
    } else {
        box_container->w += createbtn->canvas.w;
    }
    box_container->h += new_input.inputbox.canvas.h + createbtn->canvas.h;
    new_input.init = true;
    *newplaylist_input = new_input;
}

void maud_playlistmanager_newplaylist_input_displayall(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_newplaylist_input_t* new_playlistinput = &playlist_manager->new_playlistinput;
    maud_playlistmanager_newplaylist_input_init(maud, new_playlistinput);
    maud_playlistmanager_newplaylist_input_display(maud, new_playlistinput);
    maud_playlistmanager_newplaylist_input_handlebtns(maud, new_playlistinput);
    maud_playlistmanager_newplaylist_input_display_validation(maud, new_playlistinput);
}

void maud_playlistmanager_newplaylist_input_display(maud_t* maud,
    maud_newplaylist_input_t* new_playlistinput) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_buttonbar_t* button_bar = &playlist_manager->button_bar;
    maud_newplaylistbtn_t* new_playlistbtn = &button_bar->new_playlistbtn;
    maud_inputbox_t* inputbox = &new_playlistinput->inputbox;
    maud_createplaylistbtn_t* createbtn = &new_playlistinput->create_playlistbtn;
    if(!new_playlistbtn->clicked) {
        return;
    }
    inputbox->canvas.x = new_playlistbtn->canvas.x - (inputbox->canvas.w -
        new_playlistbtn->canvas.w) / 2;
    inputbox->canvas.y = new_playlistbtn->canvas.y + new_playlistbtn->canvas.h + 5;

    SDL_Rect* box_container = &new_playlistinput->canvas;
    box_container->x = new_playlistbtn->canvas.x;
    box_container->y = new_playlistbtn->canvas.y + new_playlistbtn->canvas.h;

    inputbox->canvas.x = box_container->x + (box_container->w -
        inputbox->canvas.w) / 2;
    inputbox->canvas.y = box_container->y + 50;

    SDL_Rect* placeholder_canvas = &inputbox->placeholder_canvas;
    placeholder_canvas->x = inputbox->canvas.x + 10;
    placeholder_canvas->y = inputbox->canvas.y + (inputbox->canvas.h
        - placeholder_canvas->h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(new_playlistinput->color));
    SDL_RenderDrawRect(maud->renderer, box_container);
    SDL_RenderFillRect(maud->renderer, box_container);
    maud_inputbox_display(maud, inputbox);

    SDL_Rect *createbtn_canvas = &createbtn->canvas,
             *createbtn_textcanvas = &createbtn->text.text_canvas;
    createbtn_canvas->x = inputbox->canvas.x + (inputbox->canvas.w -
        createbtn_canvas->w)/2;
    createbtn_canvas->y = inputbox->canvas.y + inputbox->canvas.h + 25;

    createbtn_textcanvas->x = createbtn_canvas->x + (createbtn_canvas->w -
        createbtn_textcanvas->w) / 2;
    createbtn_textcanvas->y = createbtn_canvas->y + (createbtn_canvas->h -
        createbtn_textcanvas->h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(createbtn->color));
    SDL_RenderDrawRect(maud->renderer, createbtn_canvas);
    SDL_RenderFillRect(maud->renderer, createbtn_canvas);

    SDL_Texture* createtext_texture = maud_textmanager_rendertext(maud, maud->font, &createbtn->text);
    SDL_RenderCopy(maud->renderer, createtext_texture, NULL, createbtn_textcanvas);
    SDL_DestroyTexture(createtext_texture); createtext_texture = NULL;
}

void maud_playlistmanager_newplaylist_input_handlebtns(maud_t* maud,
    maud_newplaylist_input_t* new_playlistinput) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_newplaylistbtn_t* new_playlistbtn = &playlist_manager->button_bar.new_playlistbtn;
    maud_inputbox_t* inputbox = &new_playlistinput->inputbox;
    if(!new_playlistbtn->clicked) {
        return;
    }
    if(maud->mouse_clicked && !maud_rect_hover(maud, new_playlistinput->canvas)) {
        new_playlistbtn->clicked = false;
        maud->mouse_clicked = false;
        maud_inputbox_clear(inputbox);
    }
    bool *playlistname_exists = &new_playlistinput->playlistname_exists,
         *playlistname_empty = &new_playlistinput->playlistname_empty;
    *playlistname_exists = false;
    if(inputbox->input.data) {
        char* playlist_name = maud_inputbox_getinputdata(inputbox);
        *playlistname_exists = maud_playlistmanager_playlistexists(maud,
            playlist_name);
        *playlistname_empty = false;
        free(playlist_name);
    }

    maud_createplaylistbtn_t* createbtn = &new_playlistinput->create_playlistbtn;
    if(maud_rect_hover(maud, createbtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            if(inputbox->input.data && !(*playlistname_exists)) {
                maud_playlistmanager_createplaylist(maud, inputbox->input.data);
                maud_inputbox_clear(inputbox);
                new_playlistbtn->clicked = false;
            } else if(!inputbox->input.data) {
                *playlistname_empty = true;
            }
            createbtn->clicked = true;
            maud->mouse_clicked = false;
        }
    }
}


void maud_playlistmanager_newplaylist_input_display_validation(maud_t* maud,
    maud_newplaylist_input_t* new_playlistinput) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_newplaylistbtn_t* new_playlistbtn = &playlist_manager->button_bar.new_playlistbtn;
    maud_createplaylistbtn_t* createbtn = &new_playlistinput->create_playlistbtn;
    maud_inputbox_t* inputbox = &new_playlistinput->inputbox;
    int playlistname_exists = new_playlistinput->playlistname_exists,
        playlistname_empty = new_playlistinput->playlistname_empty;
    if(!new_playlistbtn->clicked) {
        return;
    }
    // Whenever the playlist btn status is set equal to false this function never performs the rest of
    // the operations
    if(!createbtn->clicked) {
        return;
    }
    char* validation_msgs[] = {
        "Please enter a valid playlist name!",
        "Please choose another name the playlist already exists!"
    };
    size_t validation_index = 0;
    // Whenever the playlist name was not empty we reset createbtn clicked to false
    // otherwise its the clicked state remains true and we render the validation message to the screen
    if(playlistname_exists) {
        validation_index = 1;
    }
    maud_tooltip_t popup_tooltip = {
        .background_color = {0x12, 0x12, 0x12, 0xff},
        .delay_secs = 0.5,
        .duration_secs = 0,
        .element_canvas = inputbox->canvas,
        .font = maud->music_font,
        .margin_x = 10,
        .margin_y = 10,
        .text = validation_msgs[validation_index],
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .font_size = 18,
        .wrap_length = maud->win_width-10,
        .wrap_spacing = 10,
        .x = 0,
        .y = 0
    };
    // Whenever the playlist name was not empty we reset createbtn clicked to false
    // otherwise its the clicked state remains true and we render the validation message to the screen
    bool disable_createbtnclicked = (
        !playlistname_empty &&
        !playlistname_exists
    );
    if(disable_createbtnclicked) {
        createbtn->clicked = false;
    }
    // Get the size of the tooltip width and height and let it go down below the input box
    maud_tooltip_getsize(&popup_tooltip);
    popup_tooltip.x = 1;
    popup_tooltip.y = inputbox->canvas.y + inputbox->canvas.h;
    maud_tooltip_render(maud, &popup_tooltip);
}