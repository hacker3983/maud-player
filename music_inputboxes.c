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

bool mplayer_inputdata_realloc_datalist(mplayer_inputdata_t** input_datalist, size_t* input_datacount) {
    mplayer_inputdata_t* new_inputdatalist = NULL;
    if(!(*input_datalist) || !(*input_datacount)) {
        new_inputdatalist = (mplayer_inputdata_t*)malloc(sizeof(mplayer_inputdata_t));
    } else {
        new_inputdatalist = (mplayer_inputdata_t*)realloc(*input_datalist,
            ((*input_datacount)+1) * sizeof(mplayer_inputdata_t));
    }
    if(!new_inputdatalist) {
        return false;
    }
    *input_datalist = new_inputdatalist;
    (*input_datacount)++;
    return true;
}

bool mplayer_inputbox_realloc_datalist(mplayer_inputbox_t* input_box) {
    mplayer_inputdata_t* new_inputdatalist = NULL;
    if(!mplayer_inputdata_realloc_datalist(&input_box->input_datalist, &input_box->input_datacount)) {
        return false;
    }
    return true;
}

bool mplayer_inputbox_realloc_datalistby(mplayer_inputbox_t* input_box, size_t amount) {
    mplayer_inputdata_t* new_ptr = NULL;
    if(!input_box->input_datalist) {
        new_ptr = (mplayer_inputdata_t*)malloc(amount * sizeof(mplayer_inputdata_t));
    } else {
        new_ptr = (mplayer_inputdata_t*)realloc(input_box->input_datalist,
            (input_box->input_datacount + amount) * sizeof(mplayer_inputdata_t));
    }
    if(!new_ptr) {
        return false;
    }
    input_box->input_datalist = new_ptr;
    input_box->input_datacount += amount;
    return true;
}

bool mplayer_inputbox_parseinput_utf8(mplayer_inputbox_t* input_box, const char* input_data) {
    if(!input_data) {
        return false;
    }
    size_t parsed_datacounter = 0;
    size_t input_datalen = strlen(input_data);
    for(size_t i=0;i<input_datalen;i++) {
        char* parsed_utf8string = NULL;
        size_t parsed_utf8stringlen = 0;
        // Check if the current byte of the input data is ascii
        if(mplayer_isascii(input_data[i])) {
            // We get the starting index and the ending index of the ascii characters
            size_t ascii_startindex = i, ascii_endindex = i+1;
            i++;
            while(i < input_datalen && mplayer_isascii(input_data[i])) {
                ascii_endindex = i+1;
                i++;
            }
            // whenever we get the starting and the ending index we get the parsed utf8 string length
            // by subtracting the ascii_endindex from the starting ascii index then we store the parsed string
            // starting from the ascii_start index in the input string to the ascii_endindex in the input string
            parsed_utf8stringlen = ascii_endindex - ascii_startindex;
            parsed_utf8string = (char*)malloc(parsed_utf8stringlen+1);
            if(!parsed_utf8string) {
                return false;
            }
            // We copy the parsed result into parsed_utf8string and place it into the input data list
            strncpy(parsed_utf8string, input_data+ascii_startindex, parsed_utf8stringlen);
            parsed_utf8string[parsed_utf8stringlen] = '\0';
            // Ensure that if malloc or realloc fails then we free memory to avoid memory leaks
            if(!mplayer_inputdata_realloc_datalist(&input_box->parsed_input_datalist, &input_box->parsed_datacount)) {
                free(parsed_utf8string); parsed_utf8string = NULL;
                return false;
            }
            input_box->parsed_input_datalist[parsed_datacounter].data = parsed_utf8string;
            input_box->parsed_input_datalist[parsed_datacounter].datatype = MPLAYER_INPUTBOXDATA_ASCII;
            input_box->parsed_input_datalist[parsed_datacounter].cursor_pos = 1;
            input_box->parsed_input_datalist[parsed_datacounter++].datalen = parsed_utf8stringlen;
            input_box->parsed_datacount = parsed_datacounter;
            i--; continue;
        }
        // whenever the current character or byte is not a ascii character then we get the sequence of bytes
        // in parsed_utf8string that make up that utf8 character
        parsed_utf8string = mplayer_getutf8_char(input_data, &i, input_datalen);
        if(!parsed_utf8string) {
            return false;
        }
        parsed_utf8stringlen = strlen(parsed_utf8string);
        // Ensure that if malloc or realloc fails then we free memory to avoid memory leak
        if(!mplayer_inputdata_realloc_datalist(&input_box->parsed_input_datalist, &input_box->parsed_datacount)) {
            free(parsed_utf8string); parsed_utf8string = NULL;
            return false;   
        }
        input_box->parsed_input_datalist[parsed_datacounter].data = parsed_utf8string;
        input_box->parsed_input_datalist[parsed_datacounter].datatype = MPLAYER_INPUTBOXDATA_OTHER;
        input_box->parsed_input_datalist[parsed_datacounter].cursor_pos = 1;
        input_box->parsed_input_datalist[parsed_datacounter++].datalen = parsed_utf8stringlen;
        input_box->parsed_datacount = parsed_datacounter;
    }
    return true;
}

bool mplayer_inputbox_initialize_input_datalist(mplayer_inputbox_t* input_box) {
    // If the input box is already initialized then we do not want to reinitialize it
    if(input_box->input_datalist || input_box->input_datacount) {
        return true;
    }
    // Allocate memory so we can initialize the input data list from the parsed results    
    if(!mplayer_inputbox_realloc_datalistby(input_box, input_box->parsed_datacount)) {
        return false;
    }
    // Copy the parsed result of utf8 data into the input box datalist
    for(size_t i=0;i<input_box->parsed_datacount;i++) {
        input_box->input_datalist[i].data = input_box->parsed_input_datalist[i].data;
        input_box->input_datalist[i].datalen = input_box->parsed_input_datalist[i].datalen;
        input_box->input_datalist[i].datatype = input_box->parsed_input_datalist[i].datatype;
        input_box->input_datalist[i].cursor_pos = input_box->parsed_input_datalist[i].cursor_pos;
    }
    return true;
}

bool mplayer_inputbox_addinput_data(mplayer_inputbox_t* input_box, const char* input_data) {
    // Whenever we fail to parse the input data then we return false to the caller
    if(!mplayer_inputbox_parseinput_utf8(input_box, input_data)) {
        return false;
    }
    // Whenever the input data is empty then we just populate the input data list
    // with the parsed utf8 input data
    if(!input_box->input_datalist) {
        if(!mplayer_inputbox_initialize_input_datalist(input_box)) {
            return false;
        }
        free(input_box->parsed_input_datalist);
        input_box->parsed_input_datalist = NULL;
        input_box->parsed_datacount = 0;
        return true;
    }
    //printf("Parsed_datacount: %zu\n", input_box->parsed_datacount);
    for(size_t i=0;i<input_box->parsed_datacount;i++) {
        mplayer_inputdata_t* input_datalist = input_box->input_datalist,
            *parsed_datalist = input_box->parsed_input_datalist;
        size_t datalist_cursorpos = input_box->input_datacursor;
        // If the datatype at the last cursor insertion index is equal to a datatype of ascii
        // then we insert the new ascii string into the that data list index
        if(parsed_datalist[i].datatype == MPLAYER_INPUTBOXDATA_ASCII &&
            input_datalist[datalist_cursorpos].datatype == MPLAYER_INPUTBOXDATA_ASCII) {
                // Whenever a memory allocation failure occurs we just return false
                size_t new_datasize = input_datalist[datalist_cursorpos].datalen + 1;
                char* new_data = (char*)calloc(new_datasize+1, sizeof(char));
                // Copy the data before the cursor position into the new data buffer
                if(input_datalist[datalist_cursorpos].cursor_pos > 0) {
                    strncpy(new_data, input_datalist[datalist_cursorpos].data,
                        input_datalist[datalist_cursorpos].cursor_pos);
                }
                // Insert the parsed data into the new data buffer
                strcat(new_data, parsed_datalist[i].data);
                // Insert the data that was after the cursor position into the new data buffer
                strcat(new_data, input_datalist[datalist_cursorpos].data
                    + input_datalist[datalist_cursorpos].cursor_pos);
                free(input_datalist[datalist_cursorpos].data);
                input_datalist[datalist_cursorpos].data = new_data;
                input_datalist[datalist_cursorpos].cursor_pos++;
                input_datalist[datalist_cursorpos].datalen++;
        } else if(parsed_datalist[i].datatype == MPLAYER_INPUTBOXDATA_OTHER) {
            mplayer_inputbox_realloc_datalist(input_box);
            mplayer_inputbox_shiftdatalist_fromcursorpos(input_box);
            input_box->input_datalist[datalist_cursorpos+1].data = parsed_datalist[i].data;
            input_box->input_datalist[datalist_cursorpos+1].datalen = parsed_datalist[i].datalen;
            input_box->input_datalist[datalist_cursorpos+1].datatype = parsed_datalist[i].datatype;
            input_box->input_datalist[datalist_cursorpos+1].cursor_pos = parsed_datalist[i].cursor_pos;
        }
    }
    mplayer_inputdata_destroy(&input_box->parsed_input_datalist, &input_box->parsed_datacount);
    return true;
}

void mplayer_inputbox_shiftdatalist_fromcursorpos(mplayer_inputbox_t* input_box) {
    if(input_box->input_datacursor == input_box->input_datacount) {
        return;
    }
    size_t start_index = (input_box->input_datacount > 1) ? input_box->input_datacount-2 : 0;
    size_t j = input_box->input_datacount-1;
    for(size_t i=start_index;i>input_box->input_datacursor;i-=2) {
        mplayer_inputdata_t* input_datalist = input_box->input_datalist;
        size_t cursor_pos = input_datalist[i].cursor_pos,
           datalen = input_datalist[i].datalen;
        int datatype = input_datalist[i].datatype;
        char* data = input_datalist[i].data;
        printf("Shifting data: %s\n", data);

        input_datalist[j].data = data;
        input_datalist[j].datalen = datalen;
        input_datalist[j].datatype = datatype;
        input_datalist[j].cursor_pos = cursor_pos;
        j--;
    }
}

void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(input_box->input_datalist) {
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
    if(!input_box->input_datalist) {
        return;
    }
    int text_alignment = 4;
    size_t* datalist_startrenderindex = &input_box->start_renderindex[0],
           *data_startrenderindex = &input_box->start_renderindex[1];
    text_info_t inputdata_textinfo = {
        .font_size = 20,
        .text = NULL,
        .text_canvas = {0},
        .text_color = input_box->input_datacolor,
        .utext = input_box->text_portion
    };
    // This will store the part of the text that we should render
    char* text_portion = NULL;
    if(input_box->calculate_renderpos) {
        mplayer_inputdata_t datalist = input_box->input_datalist[*datalist_startrenderindex];
        char* current_data = datalist.data;
        int datatype = datalist.datatype;
        size_t datalen = datalist.datalen;
        int ascii_textw = 0;
        char* new_data = NULL;
        size_t new_datasize = 0, CHUNK_SIZE = 255, TOTAL_CHUNKSIZE = 255,
        j = *data_startrenderindex;
        while(j < datalen) {
            char* new_ptr = NULL;
            if(!new_data) {
                new_ptr = calloc(TOTAL_CHUNKSIZE+1, sizeof(char));
                if(!new_ptr) {
                    break;
                }
                new_data = new_ptr;
            } else {
                new_ptr = realloc(new_data, TOTAL_CHUNKSIZE + CHUNK_SIZE + 1);
                memset(new_ptr+TOTAL_CHUNKSIZE, 0, CHUNK_SIZE+1);
                TOTAL_CHUNKSIZE += CHUNK_SIZE;
                if(!new_ptr) {
                    break;
                }
                new_data = new_ptr;
            }
            TTF_SetFontSize(input_box->input_datafont, 20);
            char current_char[2] = {0};
            current_char[0] = current_data[j];
            int temp_width = 0;
            TTF_SizeUTF8(input_box->input_datafont, current_char, &temp_width, NULL);
            new_data[new_datasize++] = current_data[j];
            if(ascii_textw + temp_width > input_box->inputbox_canvas.w - 5) {
                break;
            }
            ascii_textw += temp_width;
            j++;
        }
        if(datatype == MPLAYER_INPUTBOXDATA_ASCII) {
            mplayer_concatstr(&text_portion, new_data);
        }
        input_box->text_portion = text_portion;
        input_box->calculate_renderpos = false;
        free(new_data);
    }
    SDL_Texture* inputdata_texture = mplayer_textmanager_renderunicode(mplayer, input_box->input_datafont,
        &inputdata_textinfo);
    inputdata_textinfo.text_canvas.x = input_box->inputbox_canvas.x + text_alignment;
    inputdata_textinfo.text_canvas.y = input_box->inputbox_canvas.y + (input_box->inputbox_canvas.h -
            inputdata_textinfo.text_canvas.h) / 2;
    SDL_RenderCopy(mplayer->renderer, inputdata_texture, NULL, &inputdata_textinfo.text_canvas);
    SDL_DestroyTexture(inputdata_texture); inputdata_texture = NULL;
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
    if(!music_addplaylistbtn.clicked) {
        return;
    }
    mplayer_inputdata_t *datalist = NULL;
    size_t datalen = 0, *datalist_renderindex = NULL, *data_renderindex = NULL;
    char* current_data = NULL;
    switch(mplayer->e.type) {
        case SDL_TEXTINPUT:
            mplayer_inputbox_addinput_data(input_box, mplayer->e.text.text);
            // Initialize the pointers for the datalist
            datalist = input_box->input_datalist;
            datalist_renderindex = &input_box->start_renderindex[0],
            data_renderindex = &input_box->start_renderindex[1];
            datalen = datalist[(*datalist_renderindex)].datalen;
            printf("mplayer->cursor: %zu\n", *data_renderindex);
            current_data = datalist[*datalist_renderindex].data;
            int ascii_textw = 0;
            while(*data_renderindex < datalen) {
                TTF_SetFontSize(input_box->input_datafont, 20);
                TTF_SizeUTF8(input_box->input_datafont, current_data + (*data_renderindex), &ascii_textw, NULL);
                if(ascii_textw > input_box->inputbox_canvas.w - 5) {
                    (*data_renderindex)++;
                } else {
                    break;
                }
            }

            // Process the input received so that we can determine where to render the text within the input box
            // so that it does not go outside of the input box
            input_box->calculate_renderpos = true;
            for(size_t i=0;i<input_box->input_datacount;i++) {
                printf("input_datalist[%zu] = %s\n", i, input_box->input_datalist[i].data);
            }
            printf("input_datacount = %zu\n", input_box->input_datacount);
            break;
        case SDL_KEYDOWN:
            if(!input_box->input_datalist) {
                break;
            }
            datalist = input_box->input_datalist;
            datalist_renderindex = &input_box->start_renderindex[0],
            data_renderindex = &input_box->start_renderindex[1],
            datalen = datalist[*datalist_renderindex].datalen; 
            current_data = datalist[*datalist_renderindex].data;
            if(mplayer->e.key.keysym.sym == SDLK_LEFT) {
                if(input_box->input_datalist[input_box->input_datacursor].cursor_pos) {
                    input_box->input_datalist[input_box->input_datacursor].cursor_pos--;
                    if(*data_renderindex) {
                        (*data_renderindex)--;
                        input_box->calculate_renderpos = true;
                    }
                    printf("Going left. cursor position becomes: %zu\n", input_box->input_datalist[input_box->input_datacursor].cursor_pos);
                }
            } else if(mplayer->e.key.keysym.sym == SDLK_RIGHT) {
                if(input_box->input_datalist[input_box->input_datacursor].cursor_pos <
                    input_box->input_datalist[input_box->input_datacursor].datalen) {
                    int ascii_textw = 0;
                    TTF_SetFontSize(input_box->input_datafont, 20);
                    TTF_SizeUTF8(input_box->input_datafont, current_data+(*data_renderindex), &ascii_textw, NULL);
                    if(ascii_textw > input_box->inputbox_canvas.w - 5) {
                        (*data_renderindex)++;
                    }                   
                    input_box->input_datalist[input_box->input_datacursor].cursor_pos++;
                    input_box->calculate_renderpos = true;
                    printf("Going right. cursor position becomes: %zu\n", input_box->input_datalist[input_box->input_datacursor].cursor_pos);
                }
            }
            break;
    }
}

void mplayer_inputbox_clear(mplayer_inputbox_t* input_box) {
    printf("Clearing out data\n");
    mplayer_inputdata_destroy(&input_box->input_datalist, &input_box->input_datacount);
    input_box->input_datacursor = 0;
    memset(input_box->start_renderindex, 0, 2 * sizeof(size_t));
    printf("input_box->start_renderindex[0]: %zu, input_box->start_renderindex[1]: %zu\n", input_box->start_renderindex[0],
    input_box->start_renderindex[1]);
    free(input_box->text_portion); input_box->text_portion = NULL;
}

void mplayer_inputdata_destroy(mplayer_inputdata_t** input_datalist, size_t* input_datacount) {
    for(size_t i=0;i<*input_datacount;i++) {
        free((*input_datalist)[i].data); (*input_datalist)[i].data = NULL;
        (*input_datalist)[i].datalen = 0;
        (*input_datalist)[i].cursor_pos = 0;
    }
    free(*input_datalist); *input_datalist = NULL;
    *input_datacount = 0;
}

void mplayer_inputbox_destroy(mplayer_inputbox_t* input_box) {
    mplayer_inputdata_destroy(&input_box->input_datalist, &input_box->input_datacount);
    free(input_box->input_datalist); input_box->input_datalist = NULL;
    free(input_box->placeholder); input_box->placeholder = NULL;
}