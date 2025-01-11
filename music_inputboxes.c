#include "music_inputboxes.h"

mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* input_datafont, TTF_Font* placeholder_font,
    SDL_Rect inputbox_canvas, const char* placeholder, SDL_Color placeholder_color, SDL_Color inputbox_color,
    SDL_Color input_datacolor,
    SDL_Color cursor_color) {
    mplayer_inputbox_t new_inputbox = {0};
    size_t placeholder_length = (!placeholder) ? 0 : strlen(placeholder);
    char* new_placeholder = NULL;
    if(placeholder) {
        new_placeholder = mplayer_dupstr(placeholder, placeholder_length);
    }
    new_inputbox.placeholder = new_placeholder;
    new_inputbox.inputbox_canvas = inputbox_canvas;
    new_inputbox.placeholder_color = placeholder_color;
    new_inputbox.inputbox_color = inputbox_color;
    new_inputbox.input_datacolor = input_datacolor;
    new_inputbox.input_datafont = input_datafont;
    new_inputbox.placeholder_font = placeholder_font;
    new_inputbox.cursor_color = cursor_color;
    return new_inputbox;
}

bool mplayer_inputdata_addbyte_position(input_data_t* input, size_t start_index, size_t end_index) {
    size_t* new_bytepositions = NULL;
    size_t new_len = 0, old_len = input->byte_positionlen;
    if(!input->byte_positions) {
        new_len = 2;
        new_bytepositions = malloc(2 * sizeof(size_t));
    } else {
        new_len = old_len + 2;
        new_bytepositions = realloc(input->byte_positions, new_len * sizeof(size_t));
    }
    if(!new_bytepositions) {
        return false;
    }
    new_bytepositions[old_len++] = start_index;
    new_bytepositions[old_len++] = end_index;
    input->byte_positions = new_bytepositions;
    input->byte_positionlen = new_len;
    return true;
}

bool mplayer_inputbox_getinput_beforecursor(mplayer_inputbox_t* input_box, input_data_t* input_before) {
    input_data_t* input = &input_box->input,
        new_input = {0};
    if(!input->data || !input->cursor_pos) {
        *input_before = new_input;
        return true;
    }
    // byte positions is made up of two positions for each utf8 character
    // which is a pair of the starting and ending position of those bytes
    size_t cursor_pos = input->cursor_pos, cursor_bytepos = 2 * cursor_pos;
    for(size_t i=0;i<cursor_bytepos;i+=2) {
        size_t start_index = input->byte_positions[i], end_index = input->byte_positions[i+1],
               utf8_charsize = end_index - start_index;
        char* utf8_char = malloc(utf8_charsize + 1);
        strncpy(utf8_char, input->data+start_index, utf8_charsize);
        utf8_char[utf8_charsize] = '\0';
        mplayer_concatstr(&new_input.data, utf8_char);
        mplayer_inputdata_addbyte_position(&new_input, start_index, end_index);
        new_input.datalen++;
        new_input.cursor_pos++;
        new_input.datasize += utf8_charsize;
        free(utf8_char);
    }
    *input_before = new_input;
    return true;
}

bool mplayer_inputbox_getinput_aftercursor(mplayer_inputbox_t* input_box, input_data_t* input_after) {
    input_data_t* input = &input_box->input,
        new_input = {0};
    if(!input->data || input->cursor_pos == input->datalen) {
        *input_after = new_input;
        return true;
    }
    size_t cursor_pos = input->cursor_pos, cursor_bytepos = 2 * cursor_pos;
    for(size_t i=cursor_bytepos;i<input->byte_positionlen;i+=2) {
        size_t start_index = input->byte_positions[i], end_index = input->byte_positions[i+1],
            utf8_charsize = end_index - start_index;
        char* utf8_char = malloc(utf8_charsize + 1);
        strncpy(utf8_char, input->data + start_index, utf8_charsize);
        utf8_char[utf8_charsize] = '\0';
        printf("Input after_char: %s\n", utf8_char);
        mplayer_concatstr(&new_input.data, utf8_char);
        mplayer_inputdata_addbyte_position(&new_input, start_index, end_index);
        new_input.cursor_pos++;
        new_input.datalen++;
        new_input.datasize += utf8_charsize;
        free(utf8_char);
    }
    printf("input->after = %s\n", new_input.data);
    *input_after = new_input;
    return true;
}

void mplayer_inputdata_merge(input_data_t* destination, input_data_t* source) {
    size_t new_bytepositionlen = destination->byte_positionlen + source->byte_positionlen;
    size_t* new_bytepositions = realloc(destination->byte_positions, new_bytepositionlen * sizeof(size_t));
    if(!new_bytepositions) {
        return;
    }
    if(!mplayer_concatstr(&destination->data, source->data)) {
        new_bytepositions = realloc(new_bytepositions, destination->byte_positionlen * sizeof(size_t));
        return;
    }
    for(size_t i=0;i<source->byte_positionlen;i+=2) {
        size_t start_idx = source->byte_positions[i],
               end_idx = source->byte_positions[i+1],
               utf8_charsize = end_idx - start_idx,
               new_startidx = destination->datasize,
               new_endidx = destination->datasize + utf8_charsize;
        new_bytepositions[destination->byte_positionlen + i] = new_startidx,
        new_bytepositions[destination->byte_positionlen + i + 1] = new_endidx;
        destination->datasize += utf8_charsize;
    }
    destination->datalen += source->datalen;
    destination->byte_positions = new_bytepositions;
    destination->byte_positionlen = new_bytepositionlen;
}

bool mplayer_inputbox_addinput_data(mplayer_inputbox_t* input_box, const char* utf8_input) {
    input_data_t* input = &input_box->input,
        input_before = {0},
        input_after = {0};
    size_t utf8_inputsize = strlen(utf8_input);
    mplayer_inputbox_getinput_beforecursor(input_box, &input_before);
    printf("Input before is now %s, datalen = %zu\n", input_before.data, input_before.datalen);
    mplayer_inputbox_getinput_aftercursor(input_box, &input_after);
    printf("input after is now %s, datalen = %zu\n", input_after.data, input_after.datalen);
    for(size_t i=0;i<utf8_inputsize;i++) {
        char* utf8_char = mplayer_getutf8_char(utf8_input, &i, utf8_inputsize);
        size_t utf8_charsize = strlen(utf8_char);
        mplayer_concatstr(&input_before.data, utf8_char);
        mplayer_inputdata_addbyte_position(&input_before, input_before.datasize,
            input_before.datasize+utf8_charsize);
        input_before.cursor_pos++;
        input_before.datalen++;
        input_before.datasize += utf8_charsize;
        free(utf8_char);
    }
    mplayer_inputdata_merge(&input_before, &input_after);
    mplayer_inputdata_destroy(input);
    mplayer_inputdata_destroy(&input_after);
    input_box->input = input_before;
    printf("Input: %s\n", input_box->input.data);
    return true;
}

void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(input_box->input.data) {
        return;
    }
    text_info_t placeholder_textinfo = {
        .font_size = 20,
        .text = input_box->placeholder,
        .text_canvas = {0},
        .text_color = input_box->placeholder_color,
        .utext = NULL
    };
    SDL_Texture* placeholder_texture = mplayer_textmanager_rendertext(mplayer, input_box->placeholder_font,
        &placeholder_textinfo);
    placeholder_textinfo.text_canvas.x = input_box->inputbox_canvas.x + 10;
    placeholder_textinfo.text_canvas.y = input_box->inputbox_canvas.y + 10;
    SDL_RenderCopy(mplayer->renderer, placeholder_texture, NULL, &placeholder_textinfo.text_canvas);
    SDL_DestroyTexture(placeholder_texture); placeholder_texture = NULL;    
}

void mplayer_inputbox_renderinputdata(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    
}

void mplayer_inputbox_rendercursor(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    // Perform some calculations to keep the cursor within the input box
}

void mplayer_inputbox_display(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    // Render the actual input box
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(input_box->inputbox_color));
    SDL_RenderDrawRect(mplayer->renderer, &input_box->inputbox_canvas);
    SDL_RenderFillRect(mplayer->renderer, &input_box->inputbox_canvas);

    // Render the elements within the input box
    mplayer_inputbox_renderplaceholder(mplayer, input_box);
    mplayer_inputbox_renderinputdata(mplayer, input_box);
    mplayer_inputbox_rendercursor(mplayer, input_box);
}

void mplayer_inputbox_handleinputs(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    char* current_data = NULL;
    switch(mplayer->e.type) {
        case SDL_TEXTINPUT:
            if(mplayer_inputbox_addinput_data(input_box, mplayer->e.text.text)) {
                printf("Success\n");
            } else {
                printf("Failure\n");
            }
            break;
        case SDL_KEYDOWN:
            switch(mplayer->e.key.keysym.sym) {
                case SDLK_LEFT:
                    if(input_box->input.cursor_pos) {
                        input_box->input.cursor_pos--;
                    }
                    break;
                case SDLK_RIGHT:
                    if(input_box->input.cursor_pos < input_box->input.datalen) {
                        input_box->input.cursor_pos++;
                    }
                    break;
            }
            printf("input_box->input.cursor_pos becomes %zu\n", input_box->input.cursor_pos);        
            break;
    }
}

void mplayer_inputdata_destroy(input_data_t* input) {
    free(input->data); input->data = NULL;
    free(input->byte_positions); input->byte_positions = NULL;
    input->datalen = 0;
    input->datasize = 0;
    input->cursor_pos = 0;
}

void mplayer_inputbox_clear(mplayer_inputbox_t* input_box) {
    input_data_t *input = &input_box->input;
    mplayer_inputdata_destroy(input);
}

void mplayer_inputbox_destroy(mplayer_inputbox_t* input_box) {
    mplayer_inputbox_clear(input_box);
    free(input_box->placeholder); input_box->placeholder = NULL;
}