#include "music_inputboxes.h"
#include "music_notification.h"

mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* input_datafont, TTF_Font* placeholder_font,
    SDL_Rect inputbox_canvas, const char* placeholder, SDL_Color placeholder_color, SDL_Color inputbox_color,
    SDL_Color input_selectioncolor,
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
    new_inputbox.temp_canvas = inputbox_canvas;
    new_inputbox.placeholder_color = placeholder_color;
    new_inputbox.inputbox_color = inputbox_color;
    new_inputbox.input_datacolor = input_datacolor;
    new_inputbox.input_datafont = input_datafont;
    new_inputbox.placeholder_font = placeholder_font;
    new_inputbox.render_placeholder = true;
    new_inputbox.inputbox_fill = true;
    new_inputbox.show_cursor = true;
    new_inputbox.cursor_color = cursor_color;
    new_inputbox.selection_color = input_selectioncolor;
    new_inputbox.blink_timeoutsecs = 0.5;
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
        mplayer_concatstr(&new_input.data, utf8_char);
        mplayer_inputdata_addbyte_position(&new_input, start_index, end_index);
        new_input.cursor_pos++;
        new_input.datalen++;
        new_input.datasize += utf8_charsize;
        free(utf8_char);
    }
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
    mplayer_inputbox_getinput_aftercursor(input_box, &input_after);
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
    return true;
}

void mplayer_inputbox_initcursor_range(mplayer_inputbox_t* input_box) {
    input_box->cursor_ranges = NULL;
    input_box->cursor_rangecount = 0;
}

void mplayer_inputbox_addcursor_range(mplayer_inputbox_t* input_box, size_t start_cursor, size_t end_cursor,
    size_t start_byteindex, size_t end_byteindex) {
    size_t* new_ptr = NULL;
    size_t new_count = input_box->cursor_rangecount + 4;
    new_ptr = realloc(input_box->cursor_ranges, new_count * sizeof(size_t));
    if(!new_ptr) {
        return;
    }
    new_ptr[input_box->cursor_rangecount] = start_cursor,
    new_ptr[input_box->cursor_rangecount+1] = end_cursor,
    new_ptr[input_box->cursor_rangecount+2] = start_byteindex,
    new_ptr[input_box->cursor_rangecount+3] = end_byteindex;
    input_box->cursor_ranges = new_ptr;
    input_box->cursor_rangecount = new_count;
}

void mplayer_inputbox_getcursor_ranges(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_data_t input = input_box->input;
    int max_width = input_box->temp_canvas.w - 5, total_w = 0;
    size_t cursor_start = 0, cursor_end = 0, start_index = 0, end_index = 0;
    TTF_SetFontSize(mplayer->music_font, 18);
    mplayer_inputbox_destroycursor_ranges(input_box);
    for(size_t i=0;i<input.byte_positionlen;i+=2) {
        size_t start_byteindex = input.byte_positions[i],
               end_byteindex = input.byte_positions[i+1],
               utf8_charsize = end_byteindex - start_byteindex;
        char* utf8_char = malloc(utf8_charsize+1);
        strncpy(utf8_char, input.data+start_byteindex, utf8_charsize);
        utf8_char[utf8_charsize] = '\0';
        int utf8_charw = 0;
        TTF_SizeUTF8(mplayer->music_font, utf8_char, &utf8_charw, NULL);
        total_w += utf8_charw;
        cursor_end++;
        if(total_w >= max_width) {
            end_index = start_byteindex+1;
            mplayer_inputbox_addcursor_range(input_box, cursor_start, cursor_end, start_index,
                end_index);
            start_index = end_index-1;
            cursor_start = cursor_end-1;
            total_w = 0;
        } else if(i == input.byte_positionlen-2 && total_w < max_width) {
            end_index = end_byteindex;
            mplayer_inputbox_addcursor_range(input_box, cursor_start, cursor_end, start_index,
                end_index);
            total_w = 0;
        }
        free(utf8_char);
    }
}

void mplayer_inputbox_printcursor_ranges(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    printf("[\n");
    for(size_t i=0;i<input_box->cursor_rangecount;i+=4) {
        size_t start_cursor = input_box->cursor_ranges[i], end_cursor = input_box->cursor_ranges[i+1],
               start_byteindex = input_box->cursor_ranges[i+2], end_byteindex = input_box->cursor_ranges[i+3];
        printf("\t{start_cursor: %zu, end_cursor: %zu, start_byteindex: %zu, end_byteindex: %zu}",
            start_cursor, end_cursor, start_byteindex, end_byteindex);
        if(i != input_box->cursor_rangecount-4) {
            printf(",\n");
        }
    }
    printf("\n]\n");
}

void mplayer_inputbox_destroycursor_ranges(mplayer_inputbox_t* input_box) {
    free(input_box->cursor_ranges);
    mplayer_inputbox_initcursor_range(input_box);
}

void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(input_box->input.data) {
        return;
    }
    if(!input_box->placeholder) {
        return;
    }
    if(!input_box->render_placeholder) {
        return;
    }
    text_info_t *placeholder_textinfo = &input_box->placeholder_info;
    SDL_Texture* placeholder_texture = mplayer_textmanager_rendertext(mplayer, input_box->placeholder_font,
        placeholder_textinfo);
    SDL_RenderCopy(mplayer->renderer, placeholder_texture, NULL, &placeholder_textinfo->text_canvas);
    SDL_DestroyTexture(placeholder_texture); placeholder_texture = NULL;
}

char* mplayer_inputbox_getrenderabledata(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_data_t input = input_box->input;
    size_t renderable_datalen = 0;
    char* renderable_data = NULL;
    for(size_t i=0;i<input_box->cursor_rangecount;i+=4) {
        size_t cursor_start = input_box->cursor_ranges[i], cursor_end = input_box->cursor_ranges[i+1],
               start_byteindex = input_box->cursor_ranges[i+2], end_byteindex = input_box->cursor_ranges[i+3];
        if(input.cursor_pos <= cursor_end && input.cursor_pos >= cursor_start) {
            renderable_datalen = end_byteindex - start_byteindex;
            renderable_data = malloc(renderable_datalen+1);
            strncpy(renderable_data, input.data+start_byteindex, renderable_datalen);
            renderable_data[renderable_datalen] = '\0';
            input_box->start_renderpos = cursor_start, input_box->end_renderpos = cursor_end;
            break;
        }
    }
    return renderable_data;
}

char* mplayer_inputbox_getselectiondata(mplayer_inputbox_t* input_box) {
    input_selection_t* selections = &input_box->selections;
    input_data_t input = input_box->input;
    char* selection_data = NULL;
    for(size_t i=0;i<selections->count;i++) {
        size_t cursor_pos = selections->list[i];
        size_t start_byteidx = input.byte_positions[(2 * cursor_pos) - 2],
               end_byteidx = input.byte_positions[(2 * cursor_pos) - 1],
               utf8_charsize = end_byteidx - start_byteidx;
        char* utf8_char = malloc(utf8_charsize+1);
        strncpy(utf8_char, input.data+start_byteidx, utf8_charsize);
        utf8_char[utf8_charsize] = '\0';
        mplayer_concatstr(&selection_data, utf8_char);
        free(utf8_char);
    }
    return selection_data;
}

void mplayer_inputbox_getselectionsize(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_selection_t* selections = &input_box->selections;
    input_data_t input = input_box->input;
    if(!input.data) {
        return;
    }
    if(input_box->select_all) {
        input_box->selection_canvas = input_box->input_text.text_canvas;
        return;
    }
    if(!selections->list) {
        return;
    }
    text_info_t temp_text = input_box->input_text;
    SDL_Rect selection_canvas = {
        .x = input_box->input_text.text_canvas.x,
        .y = input_box->input_text.text_canvas.y,
        .w = 0,
        .h = input_box->input_text.text_canvas.h
    };
    bool found_startselection = false;
    size_t cursor_start = input_box->start_renderpos, cursor_end = input_box->end_renderpos,
           cursor_counter = input_box->start_renderpos, start_selectionindex = 0;
    char* renderable_data = input_box->renderable_data;
    size_t render_index = 0;
    size_t renderable_datalen = strlen(input_box->renderable_data);
    for(size_t i=0;i<selections->count;i++) {
        size_t cursor_pos = selections->list[i];
        int direction = selections->direction;
        if(cursor_pos <= cursor_end &&
            cursor_pos >= cursor_start) {
            if(!found_startselection) {
                start_selectionindex = i;
                found_startselection = true;
            }
            size_t start_byteidx = input.byte_positions[(2 * cursor_pos) - 2],
                   end_byteidx = input.byte_positions[(2 * cursor_pos) - 1];
            size_t utf8_charsize = end_byteidx - start_byteidx;
            char* utf8_char = malloc(utf8_charsize+1);
            strncpy(utf8_char, input.data+start_byteidx, utf8_charsize);
            utf8_char[utf8_charsize] = '\0';
            temp_text.utext = utf8_char;
            printf("Selection char: %s, cursor_pos: %zu\n", utf8_char, cursor_pos);
            mplayer_textmanager_sizetext(input_box->input_datafont, &temp_text);
            selection_canvas.w += temp_text.text_canvas.w;
            free(utf8_char);
        }
    }
    for(size_t i=0,j=0;i<renderable_datalen;i++,j++) {
        char* utf8_char = mplayer_getutf8_char(renderable_data, &i, renderable_datalen);
        temp_text.utext = utf8_char;
        mplayer_textmanager_sizetext(input_box->input_datafont, &temp_text);
        if(cursor_start + j < selections->list[start_selectionindex]-1) {
            selection_canvas.x += temp_text.text_canvas.w;
        }
        free(utf8_char);
    }
    input_box->selection_canvas = selection_canvas;
}

void mplayer_inputbox_getsize(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_data_t* input = &input_box->input;
    mplayer_inputbox_getplaceholder_size(mplayer, input_box);
    text_info_t input_text = {
        .font_size = 18,
        .text = NULL,
        .text_canvas = {
            .x = input_box->inputbox_canvas.x + 5, .y = 0,
            .w = 0, .h = 0
        },
        .text_color = input_box->input_datacolor,
        .utext = NULL
    };
    input_box->input_text = input_text;
    if(!input->data) {
        return;
    }
    size_t renderable_datalen = 0;
    char *data = input->data,
         *renderable_data = (!input_box->renderable_data) ?
            mplayer_inputbox_getrenderabledata(mplayer, input_box) :
            input_box->renderable_data;
    input_text.utext = renderable_data;
    mplayer_textmanager_sizetext(input_box->input_datafont, &input_text);
    if(input_text.text_canvas.w > input_box->inputbox_canvas.w - 5) {
        input_box->inputbox_canvas.w = input_text.text_canvas.w + 6;
    }
    input_text.text_canvas.y = input_box->inputbox_canvas.y + (input_box->inputbox_canvas.h -
        input_text.text_canvas.h) / 2;
    input_box->renderable_data = renderable_data;
    input_box->input_text = input_text;
    mplayer_inputbox_getselectionsize(mplayer, input_box);
}

void mplayer_inputbox_getplaceholder_size(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(!input_box->placeholder) {
        return;
    }
    if(!input_box->render_placeholder) {
        return;
    }
    text_info_t* placeholder_textinfo = &input_box->placeholder_info;
    placeholder_textinfo->font_size = 20;
    placeholder_textinfo->text = input_box->placeholder;
    placeholder_textinfo->text_color = input_box->placeholder_color;
    placeholder_textinfo->utext = NULL;
    if(!input_box->placeholder_truncated) {
        char* truncated_placeholder = mplayer_textmanager_truncatetext(input_box->placeholder_font, placeholder_textinfo,
            input_box->inputbox_canvas.w - 10);
        if(truncated_placeholder) {
            placeholder_textinfo->text = truncated_placeholder;
        }
        input_box->placeholder_truncated = truncated_placeholder;
    }
    mplayer_textmanager_sizetext(input_box->placeholder_font, placeholder_textinfo);
}

void mplayer_inputbox_renderselection(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_selection_t* selections = &input_box->selections;
    if(!input_box->select_all && !selections->list) {
        return;
    }
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(input_box->selection_color));
    SDL_RenderDrawRect(mplayer->renderer, &input_box->selection_canvas);
    SDL_RenderFillRect(mplayer->renderer, &input_box->selection_canvas);
}

void mplayer_inputbox_renderinputdata(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_data_t* input = &input_box->input;
    if(!input->data) {
        return;
    }
    text_info_t* input_text = &input_box->input_text;
    SDL_Texture* texture = mplayer_textmanager_renderunicode(mplayer, input_box->input_datafont, input_text);
    SDL_RenderCopy(mplayer->renderer, texture, NULL, &input_text->text_canvas);
    SDL_DestroyTexture(texture);
    texture = NULL;
}

int mplayer_inputbox_getcursoroffsetwidth(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    size_t cursor_offsetwidth = 0;
    input_data_t input = input_box->input;
    text_info_t input_text = input_box->input_text;
    for(size_t i=0;i<input_box->cursor_rangecount && input.cursor_pos;i+=4) {
        size_t cursor_start = input_box->cursor_ranges[i],
               cursor_end = input_box->cursor_ranges[i+1],
               end_byteindex = input.byte_positions[(2 * input.cursor_pos)-1],
               start_byteindex = input_box->cursor_ranges[i+2];
        if(input.cursor_pos <= cursor_end && input.cursor_pos >= cursor_start) {
            for(size_t j=start_byteindex;j<end_byteindex;j++) {
                char* utf8_char = mplayer_getutf8_char(input.data, &j, input.datasize);
                input_text.utext = utf8_char;
                mplayer_textmanager_sizetext(input_box->input_datafont, &input_text);
                cursor_offsetwidth += input_text.text_canvas.w;
                free(utf8_char); utf8_char = NULL;
            }
            break;
        }
    }
    return cursor_offsetwidth;
}

void mplayer_inputbox_resetcursor_blink(mplayer_inputbox_t* input_box) {
    input_box->blink_timeout = 0;
    input_box->cursor_blink = false;
}

void mplayer_inputbox_rendercursor(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(!input_box->show_cursor) {
        return;
    } 
    input_data_t input = input_box->input;
    // Perform some calculations to keep the cursor within the input box
    text_info_t input_text = input_box->input_text;
    if(input.data) {
        int cursor_offsetwidth = mplayer_inputbox_getcursoroffsetwidth(mplayer, input_box);
        if(cursor_offsetwidth >= input_box->temp_canvas.w - 5) {
            input_box->cursor_canvas.x = input_text.text_canvas.x + input_text.text_canvas.w;
        } else {
            input_box->cursor_canvas.x = input_text.text_canvas.x + cursor_offsetwidth;
        }
        
    } else {
        input_text.utext = "A";
        mplayer_textmanager_sizetext(input_box->input_datafont, &input_text);
        input_box->cursor_canvas.x = input_box->inputbox_canvas.x + 2;
    }
    input_box->cursor_canvas.w = 2,
    input_box->cursor_canvas.h = input_text.text_canvas.h + 5;
    input_box->cursor_canvas.y = input_box->inputbox_canvas.y + (input_box->inputbox_canvas.h -
        input_box->cursor_canvas.h) / 2;
    if(!input_box->blink_timeout) {
        input_box->blink_timeout = SDL_GetTicks64() + (input_box->blink_timeoutsecs * 1000);
    }
    if(!input_box->cursor_blink) {
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(input_box->cursor_color));
        SDL_RenderDrawRect(mplayer->renderer, &input_box->cursor_canvas);
        SDL_RenderFillRect(mplayer->renderer, &input_box->cursor_canvas);
        if(SDL_GetTicks64() >= input_box->blink_timeout) {
            input_box->blink_timeout = 0;
            input_box->cursor_blink = true;
        }
        return;
    } else if(input_box->cursor_blink && SDL_GetTicks64() >= input_box->blink_timeout) {
        input_box->blink_timeout = 0;
        input_box->cursor_blink = false;
    }
}

void mplayer_inputbox_display(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    mplayer_inputbox_getsize(mplayer, input_box);
    // Render the actual input box
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(input_box->inputbox_color));
    SDL_RenderDrawRect(mplayer->renderer, &input_box->inputbox_canvas);
    if(input_box->inputbox_fill) {
        SDL_RenderFillRect(mplayer->renderer, &input_box->inputbox_canvas);
    }
    // Render the elements within the input box
    mplayer_inputbox_renderplaceholder(mplayer, input_box);
    mplayer_inputbox_renderselection(mplayer, input_box);
    mplayer_inputbox_renderinputdata(mplayer, input_box);
    mplayer_inputbox_rendercursor(mplayer, input_box);
    free(input_box->renderable_data);
    input_box->renderable_data = NULL;
    free(input_box->placeholder_truncated);
    input_box->placeholder_truncated = NULL;
}

void mplayer_inputbox_backspace(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    input_data_t before = {0},
                 after = {0},
                 input = input_box->input;
    if(!input_box->input.cursor_pos) {
        return;
    }
    input_box->input.cursor_pos--;
    mplayer_inputbox_getinput_beforecursor(input_box, &before);
    mplayer_inputbox_getinput_aftercursor(input_box, &after);
    for(size_t i=2;i<after.byte_positionlen;i+=2) {
        size_t start_index = after.byte_positions[i], end_index = after.byte_positions[i+1],
               utf8_charsize = end_index - start_index;
        size_t new_startindex = before.datasize, new_endindex = before.datasize + utf8_charsize;
        char* utf8_char = malloc(utf8_charsize+1);
        strncpy(utf8_char, input.data+start_index, utf8_charsize);
        utf8_char[utf8_charsize] = '\0';
        mplayer_concatstr(&before.data, utf8_char);
        mplayer_inputdata_addbyte_position(&before, new_startindex, new_endindex);
        before.datalen++;
        before.datasize += utf8_charsize;
        free(utf8_char);
        utf8_char = NULL;
    }
    mplayer_inputdata_destroy(&after);
    mplayer_inputdata_destroy(&input_box->input);
    input_box->input = before;
}

bool mplayer_inputbox_hover(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(mplayer_rect_hover(mplayer, input_box->inputbox_canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_TYPEABLE);
        return true;
    }
    return false;
}

bool mplayer_inputbox_clicked(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(mplayer->mouse_clicked) {
        if(mplayer_inputbox_hover(mplayer, input_box)) {
            input_box->clicked = true;
            mplayer->mouse_clicked = false;
            return true;
        }
        input_box->clicked = false;
    }
    return false;
}

void mplayer_inputbox_handleinputs(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    char* current_data = NULL;
    switch(mplayer->e.type) {
        case SDL_TEXTINPUT:
            if(input_box->select_all) {
                mplayer_inputbox_clear(input_box);
                input_box->select_all = false;
            } else if(input_box->selections.list) {
                size_t start_cursorpos = input_box->selections.list[0],
                       end_cursorpos = input_box->selections.list[input_box->selections.count-1];
                for(size_t i=end_cursorpos;i>=start_cursorpos;) {
                    input_box->input.cursor_pos = i;
                    mplayer_inputbox_backspace(mplayer, input_box);
                    if(i == start_cursorpos) {
                        break;
                    }
                    i--;
                }
                mplayer_inputbox_clearselection(input_box);
            }
            mplayer_inputbox_addinput_data(input_box, mplayer->e.text.text);
            mplayer_inputbox_getcursor_ranges(mplayer, input_box);
            mplayer_inputbox_resetcursor_blink(input_box);
            input_box->update_renderpos = true;
            break;
        case SDL_KEYDOWN:
            if(SDL_GetModState() & KMOD_CTRL) {
                switch(mplayer->e.key.keysym.sym) {
                    case SDLK_a:
                        if(input_box->input.data) {
                            mplayer_inputbox_clearselection(input_box);
                            input_box->input.cursor_pos = input_box->input.datalen;
                            input_box->select_all = true;
                        }
                        break;
                    case SDLK_c:
                        if(!input_box->input.data) {
                            break;
                        }
                        bool display_notification = false;
                        int result = 0;
                        if(input_box->select_all && input_box->input.data) {
                            display_notification = true;
                            result = SDL_SetClipboardText(input_box->input.data);
                        } else if(input_box->selections.list) {
                            display_notification = true;
                            char* selection_data = mplayer_inputbox_getselectiondata(input_box);
                            if(!selection_data) {
                                result = -1;
                            }
                            result = SDL_SetClipboardText(selection_data);
                            free(selection_data);
                        }
                        char* result_msg = (result == 0) ? "Successfully copied data to clipboard." :
                                                        "Failed to copy data to clipboard.";
                        if(display_notification) {
                            mplayer_notification_push(&mplayer->notification, mplayer->music_font, 20,
                                black, result_msg, (SDL_Color){0x00, 0xff, 0x00, 0xff},
                                1.5,
                                10,
                                10,
                                10
                            );
                        }
                        break;
                    case SDLK_v:
                        char* text = SDL_GetClipboardText();
                        if(input_box->select_all) {
                            mplayer_inputbox_clear(input_box);
                            input_box->select_all = false;
                        } else if(input_box->selections.list) {
                            size_t start_cursorpos = input_box->selections.list[0],
                                   end_cursorpos = input_box->selections.list[input_box->selections.count-1];
                            for(size_t i=end_cursorpos;i>=start_cursorpos;) {
                                input_box->input.cursor_pos = i;
                                mplayer_inputbox_backspace(mplayer, input_box);
                                if(i == start_cursorpos) {
                                    break;
                                }
                                i--;
                            }
                            mplayer_inputbox_clearselection(input_box);
                        }
                        if(text && strlen(text) > 0) {
                            mplayer_inputbox_addinput_data(input_box, text);
                            mplayer_inputbox_getcursor_ranges(mplayer, input_box);
                            mplayer_inputbox_resetcursor_blink(input_box);
                        }
                        free(text); text = NULL;
                }
                break;
            } else if(SDL_GetModState() & KMOD_SHIFT) {
                switch(mplayer->e.key.keysym.sym) {
                    case SDLK_LEFT:
                        input_box->select_all = false;
                        if(input_box->input.cursor_pos) {
                            mplayer_inputbox_addselection_left(input_box, input_box->input.cursor_pos);
                            input_box->input.cursor_pos--;
                        }
                        break;
                    case SDLK_RIGHT:
                        input_box->select_all = false;
                        if(input_box->input.cursor_pos < input_box->input.datalen) {
                            input_box->input.cursor_pos++;
                            mplayer_inputbox_addselection_right(input_box, input_box->input.cursor_pos);
                        }
                        break;
                }
                break;
            }
            switch(mplayer->e.key.keysym.sym) {
                case SDLK_LEFT:
                    if(input_box->selections.list) {
                        input_box->input.cursor_pos = input_box->selections.list[0];
                        mplayer_inputbox_clearselection(input_box);
                        break;
                    } else if(input_box->select_all) {
                        input_box->select_all = false;
                        input_box->input.cursor_pos = 0;
                        break;
                    }
                    if(input_box->input.cursor_pos) {
                        input_box->input.cursor_pos--;
                    }
                    input_box->update_renderpos = true;
                    mplayer_inputbox_resetcursor_blink(input_box);
                    break;
                case SDLK_RIGHT:
                    if(input_box->selections.list) {
                        input_box->input.cursor_pos = input_box->selections.list[input_box->selections.count-1];
                        mplayer_inputbox_clearselection(input_box);
                        break;
                    } else if(input_box->select_all) {
                        input_box->select_all = false;
                        input_box->input.cursor_pos = input_box->input.datalen;
                        break;
                    }
                    if(input_box->input.cursor_pos < input_box->input.datalen) {
                        input_box->input.cursor_pos++;
                    }
                    input_box->update_renderpos = true;
                    mplayer_inputbox_resetcursor_blink(input_box);
                    break;
                case SDLK_BACKSPACE:
                    if(input_box->select_all) {
                        mplayer_inputbox_clear(input_box);
                        input_box->select_all = false;
                        break;
                    } else if(input_box->selections.list) {
                        size_t start_cursorpos = input_box->selections.list[0],
                               end_cursorpos = input_box->selections.list[input_box->selections.count-1];
                        for(size_t i=end_cursorpos;i>=start_cursorpos;) {
                            input_box->input.cursor_pos = i;
                            mplayer_inputbox_backspace(mplayer, input_box);
                            if(i == start_cursorpos) {
                                break;
                            }
                            i--;
                        }
                        mplayer_inputbox_clearselection(input_box);
                        break;
                    }
                    mplayer_inputbox_backspace(mplayer, input_box);
                    mplayer_inputbox_getcursor_ranges(mplayer, input_box);
                    mplayer_inputbox_resetcursor_blink(input_box);
                    break;
            }
            //printf("Cursor position becomes %zu\n", input_box->input.cursor_pos);        
            break;
    }
}


void mplayer_inputbox_popselection_front(mplayer_inputbox_t* input_box) {
    input_selection_t* selections = &input_box->selections;
    if(!selections->list) {
        return;
    }
    for(size_t i=0;i<selections->count-1;i++) {
        size_t current = selections->list[i];
        selections->list[i] = selections->list[i+1],
        selections->list[i+1] = current;
    }
    selections->count--;
    selections->list = realloc(selections->list, selections->count * sizeof(size_t));
}

void mplayer_inputbox_popselection_back(mplayer_inputbox_t* input_box) {
    input_selection_t* selections = &input_box->selections;
    if(!selections->list) {
        return;
    }
    selections->count--;
    selections->list = realloc(selections->list, selections->count * sizeof(size_t));
}

void mplayer_inputbox_addselection_left(mplayer_inputbox_t* input_box, size_t cursor_pos) {
    input_selection_t* selections = &input_box->selections;
    size_t* new_list = NULL;
    size_t new_count = 0;
    // If we selected right and now we are going left then pop the cursor position at the back of the selection list
    // until the selection is empty
    if(selections->list && input_box->selections.direction != INPUT_SELECTIONLEFT) {
        mplayer_inputbox_popselection_back(input_box);
        return;
    } else if(!selections->list) {
        selections->direction = INPUT_SELECTIONLEFT;
    }
    new_count = selections->count + 1;
    new_list = realloc(selections->list, new_count * sizeof(size_t));
    if(!new_count) {
        return;
    }
    new_list[new_count-1] = cursor_pos;
    for(size_t i=new_count-1;i>0;i--) {
        size_t current = new_list[i];
        new_list[i] = new_list[i-1];
        new_list[i-1] = current;
    }
    selections->list = new_list;
    selections->count = new_count;
}

void mplayer_inputbox_addselection_right(mplayer_inputbox_t* input_box, size_t cursor_pos) {
    input_selection_t* selections = &input_box->selections;
    size_t* new_list = NULL;
    size_t new_count = 0;
    if(selections->list && input_box->selections.direction != INPUT_SELECTIONRIGHT) {
        mplayer_inputbox_popselection_front(input_box);
        return;
    } else if(!selections->list) {
        selections->direction = INPUT_SELECTIONRIGHT;
    }
    new_count = selections->count + 1;
    new_list = realloc(selections->list, new_count * sizeof(size_t));
    if(!new_count) {
        return;
    }
    new_list[new_count-1] = cursor_pos;
    selections->list = new_list;
    selections->count = new_count;
}

void mplayer_inputbox_addselection(mplayer_inputbox_t* input_box, size_t cursor_pos, int direction) {
    if(direction == INPUT_SELECTIONLEFT) {
        mplayer_inputbox_addselection_left(input_box, cursor_pos);
    } else if(direction == INPUT_SELECTIONRIGHT) {
        mplayer_inputbox_addselection_right(input_box, cursor_pos);
    }
}

void mplayer_inputbox_clearselection(mplayer_inputbox_t* input_box) {
    free(input_box->selections.list);
    input_box->selections.list = NULL;
    input_box->selections.count = 0;
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
    mplayer_inputbox_destroycursor_ranges(input_box);
    mplayer_inputbox_clearselection(input_box);
    free(input_box->renderable_data);
    input_box->renderable_data = NULL;
}

void mplayer_inputbox_destroy(mplayer_inputbox_t* input_box) {
    mplayer_inputbox_clear(input_box);
    free(input_box->placeholder); input_box->placeholder = NULL;
    free(input_box->placeholder_truncated); input_box->placeholder_truncated = NULL;
}