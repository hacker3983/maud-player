#include "music_inputboxes.h"

mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* input_datafont, TTF_Font* placeholder_font,
    SDL_Rect inputbox_canvas, const char* placeholder, SDL_Color placeholder_color, SDL_Color inputbox_color,
    SDL_Color input_datacolor) {
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
    if(!input_box->input_datalist) {
        if(!mplayer_inputbox_realloc_datalist(input_box)) {
            return false;
        }
        return true;
    }
    mplayer_inputbox_t* new_ptr = realloc(input_box->input_datalist,
                (input_box->input_datacount + amount) * sizeof(mplayer_inputdata_t));
    if(!new_ptr) {
        return false;
    }
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
            strncpy(parsed_utf8string, input_data+i, parsed_utf8stringlen);
            parsed_utf8string[parsed_utf8stringlen] = '\0';
            // Ensure that if malloc or realloc fails then we free memory to avoid memory leaks
            if(!mplayer_inputdata_realloc_datalist(&input_box->parsed_input_datalist, &input_box->parsed_datacount)) {
                free(parsed_utf8string); parsed_utf8string = NULL;
                return false;
            }
            input_box->parsed_input_datalist[parsed_datacounter].data = parsed_utf8string;
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
        input_box->parsed_input_datalist[parsed_datacounter++].datalen = parsed_utf8stringlen;
        input_box->parsed_datacount = parsed_datacounter;
    }
    return true;
}

bool mplayer_inputbox_addinput_data(mplayer_inputbox_t* input_box, const char* input_data) {
    size_t input_datalen = 0;
    char* new_inputdata = NULL; 
    // Ensure that the input_data is not NULL before getting the length of duplicating it
    // to prevent crashes / segmentation fault
    if(input_data) {
        input_datalen = strlen(input_data);
        new_inputdata = mplayer_dupstr(input_data, input_datalen);
    }
    // If malloc fails then we return that the add input data function fails
    if(!new_inputdata || !input_datalen) {
        return false;
    }
    // Whenever we fail to parse the input data then we return false to the caller
    if(!mplayer_inputbox_parseinput_utf8(input_box, input_data)) {
        return false;
    }
    size_t oldinput_datacount = input_box->input_datacount;
    // Increase the size of the actually input data list to store the parsed results
    if(!mplayer_inputbox_realloc_datalistby(input_box, input_box->parsed_datacount)) {
        return false;
    }
    // Copy the parsed results into the actual input data list
    for(size_t i=oldinput_datacount;i<input_box->input_datacount;i++) {
        input_box->input_datalist[i].cursor_pos = 0;
        input_box->input_datalist[i].data = input_box->parsed_input_datalist[i].data;
        input_box->input_datalist[i].datalen = input_box->parsed_input_datalist[i].datalen;
    }
    free(input_box->parsed_input_datalist); input_box->parsed_input_datalist = NULL;
    input_box->parsed_datacount = 0;
    return true;
}

void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
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
    text_info_t inputdata_textinfo = {
        .font_size = 20,
        .text = NULL,
        .text_canvas = {0},
        .text_color = input_box->input_datacolor,
        .utext = input_box->input_datalist[0].data
    };
    SDL_Texture* inputdata_texture = mplayer_textmanager_renderunicode(mplayer, input_box->input_datafont,
        &inputdata_textinfo);
    SDL_RenderCopy(mplayer->renderer, inputdata_texture, NULL, &inputdata_textinfo.text_canvas);
    SDL_DestroyTexture(inputdata_texture); inputdata_texture = NULL;
}

void mplayer_inputbox_display(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    // Render the actual input box
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(input_box->inputbox_color));
    SDL_RenderDrawRect(mplayer->renderer, &input_box->inputbox_canvas);
    SDL_RenderFillRect(mplayer->renderer, &input_box->inputbox_canvas);

    // Render the elements within the input box
    mplayer_inputbox_renderplaceholder(mplayer, input_box);
    //mplayer_inputbox_renderinputdata(mplayer, input_box);
}

void mplayer_inputbox_handleinputs(mplayer_t* mplayer, mplayer_inputbox_t* input_box) {
    if(mplayer->e.type != SDL_TEXTINPUT) {
        return;
    }
    mplayer_inputbox_addinput_data(input_box, mplayer->e.text.text);
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