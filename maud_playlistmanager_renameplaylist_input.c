#include "maud_playlistmanager.h"

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

}