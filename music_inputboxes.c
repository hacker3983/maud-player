#include "music_inputboxes.h"
#include "music_notification.h"

void mplayer_inputbox_init(mplayer_inputbox_t* inputbox, TTF_Font* font, int font_size, SDL_Color color,
    const char* placeholder, SDL_Color placeholder_color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width,
    int cursor_height) {
    inputbox->font = font;
    inputbox->font_size = font_size;
    inputbox->text_color = text_color;
    inputbox->canvas = (SDL_Rect) {x, y, width, height};
    inputbox->canvas_color = color;
    inputbox->cursor_color = cursor_color;
    inputbox->cursor_width = cursor_width;
    inputbox->cursor_height = cursor_height;
    if(placeholder) {
        inputbox->placeholder = mplayer_dupstr(placeholder, strlen(placeholder));
    }
    inputbox->placeholder_color = placeholder_color;
    inputbox->placeholder_show = true;
    inputbox->placeholder_canvas = (SDL_Rect) {
        .x = inputbox->canvas.x+1, .y = inputbox->canvas.y,
        .w = 0, .h = 0
    };
    inputbox->fill = true;
    inputbox->show_cursor = true;
    inputbox->blink_timeoutsecs = 0.5;
    inputbox->cursor_blink = true;
}

mplayer_inputbox_t mplayer_inputbox_create(TTF_Font* font, int font_size, SDL_Color color,
    const char* placeholder, SDL_Color placeholder_color, SDL_Color cursor_color,
    SDL_Color text_color, int x, int y, int width, int height, int cursor_width,
    int cursor_height) {
    mplayer_inputbox_t new_inputbox = {0};
    mplayer_inputbox_init(&new_inputbox, font, font_size, color, placeholder, placeholder_color,
        cursor_color, text_color, x, y, width, height, cursor_width, cursor_height);
    return new_inputbox;
}

void mplayer_inputbox_addinputchar(mplayer_inputbox_t* inputbox, char* utf8_char) {
    inputdata_t* input = &inputbox->input;
    size_t new_count = input->character_count + 1;
    inputchar_t* new_characters = realloc(input->characters, new_count * sizeof(inputchar_t));
    if(!new_characters) {
        return;
    }
    new_characters[new_count-1].utf8_char = utf8_char;
    TTF_SetFontSize(inputbox->font, inputbox->font_size);
    TTF_SizeUTF8(inputbox->font, utf8_char, &new_characters[new_count-1].canvas.w,
        &new_characters[new_count-1].canvas.h);
    new_characters[new_count-1].utf8_charlen = strlen(utf8_char);
    input->size += new_characters[new_count-1].utf8_charlen;
    for(size_t i=new_count-1;i>input->cursor_pos;i--) {
        inputchar_t current_character = new_characters[i];
        new_characters[i] = new_characters[i-1];
        new_characters[i-1] = current_character;
    }
    input->cursor_pos++;
    input->characters = new_characters;
    input->character_count = new_count;
}

void mplayer_inputbox_addinputdata(mplayer_inputbox_t* inputbox, const char* utf8_string) {
    size_t utf8_stringsize = strlen(utf8_string);
    for(size_t i=0;i<utf8_stringsize;i++) {
        char* utf8_char = mplayer_getutf8_char(utf8_string, &i, utf8_stringsize);
        mplayer_inputbox_addinputchar(inputbox, utf8_char);
    }
}

char* mplayer_inputbox_getinputdata(mplayer_inputbox_t* inputbox) {
    if(!inputbox->input.characters) {
        return NULL;
    }
    char* new_data = calloc(inputbox->input.size+1, sizeof(char));
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        strcat(new_data, inputbox->input.characters[i].utf8_char);
    }
    return new_data;
}

void mplayer_inputbox_backspace(mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(!input->characters || !input->cursor_pos) {
        return;
    }
    for(size_t i=input->cursor_pos-1;i<input->character_count-1;i++) {
        inputchar_t current_character = input->characters[i];
        input->characters[i] = input->characters[i+1];
        input->characters[i+1] = current_character;
    }
    input->size -= input->characters[input->character_count-1].utf8_charlen;
    input->cursor_pos--;
    input->character_count--;
    free(input->characters[input->character_count].utf8_char);
    if(!input->character_count) {
        free(input->characters);
        input->characters = NULL;
        return;
    }
    input->characters = realloc(input->characters, input->character_count * sizeof(inputchar_t));
}

void mplayer_inputbox_addselectionleft(mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(!input->cursor_pos) {
        return;
    }
    if(input->selection_direction == INPUT_SELECTIONRIGHT && input->selection_count) {
        input->cursor_pos--;
        input->selection_end--;
        input->selection_count--;
        return;
    }
    input->selection_direction = INPUT_SELECTIONLEFT;
    if(!input->selection_count) {
        input->selection_end = input->cursor_pos;
    }
    input->cursor_pos--;
    input->selection_start = input->cursor_pos;
    input->selection_count++;
}

void mplayer_inputbox_addselectionright(mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(input->cursor_pos == input->character_count) {
        return;
    }
    if(input->selection_direction == INPUT_SELECTIONLEFT && input->selection_count) {
        input->cursor_pos++;
        input->selection_start++;
        input->selection_count--;
        return;
    }
    input->selection_direction = INPUT_SELECTIONRIGHT;
    if(!input->selection_count) {
        input->selection_start = input->cursor_pos;
    }
    input->cursor_pos++;
    input->selection_count++;
    input->selection_end = input->selection_start + input->selection_count;
}

void mplayer_inputbox_selectall(mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(!input->characters) {
        return;
    }
    input->selection_start = 0;
    input->cursor_pos = input->character_count;
    input->selection_end = input->character_count;
    input->selection_count = input->character_count;
    input->selection_direction = INPUT_SELECTIONRIGHT;
}

char* mplayer_inputbox_getselectiondata(mplayer_inputbox_t* inputbox) {
    char* data = NULL;
    for(size_t i=inputbox->input.selection_start;i<inputbox->input.selection_end;i++) {
        mplayer_concatstr(&data, inputbox->input.characters[i].utf8_char);
    }    
    return data;
}

void mplayer_inputbox_addselection(mplayer_inputbox_t* inputbox, int direction) {
    switch(direction) {
        case INPUT_SELECTIONLEFT:
            mplayer_inputbox_addselectionleft(inputbox);
            break;
        case INPUT_SELECTIONRIGHT:
            mplayer_inputbox_addselectionright(inputbox);
            break;
    }
}

void mplayer_inputbox_clearselection(mplayer_inputbox_t* inputbox) {
    inputbox->input.selection_start = 0;
    inputbox->input.selection_end = 0;
    inputbox->input.selection_count = 0;
}

void mplayer_inputbox_printselection(mplayer_inputbox_t* inputbox) {
    if(!inputbox->input.selection_count) {
        return;
    }
    printf("Selection text: ");
    for(size_t i=inputbox->input.selection_start;i<inputbox->input.selection_end;i++) {
        printf("%s", inputbox->input.characters[i].utf8_char);
    }
    printf("\n");
}

void mplayer_inputbox_cursor_resetblink(mplayer_inputbox_t* inputbox) {
    inputbox->cursor_timeout = 0;
    inputbox->cursor_blinking = false;
}

void mplayer_inputbox_handle_events(mplayer_t* mplayer, mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    switch(mplayer->e.type) {
        case SDL_TEXTINPUT:
            if(input->selection_count) {
                input->cursor_pos = input->selection_end;
                if(input->selection_count == input->character_count) {
                    inputbox->render_pos = 0;
                }
                for(size_t i=0;i<input->selection_count;i++) {
                    mplayer_inputbox_backspace(inputbox);
                }
                mplayer_inputbox_clearselection(inputbox);
                free(inputbox->input.data);
                inputbox->input.data = mplayer_inputbox_getinputdata(inputbox);
            }
            mplayer_inputbox_addinputdata(inputbox, mplayer->e.text.text);
            mplayer_inputbox_cursor_resetblink(inputbox);
            free(inputbox->input.data);
            inputbox->input.data = mplayer_inputbox_getinputdata(inputbox);
            break;
        case SDL_KEYDOWN:
            if(SDL_GetModState() & KMOD_CTRL) {
                switch(mplayer->e.key.keysym.sym) {
                    case SDLK_a:
                        mplayer_inputbox_selectall(inputbox);
                        break;
                    case SDLK_c:
                        char* selection_data = mplayer_inputbox_getselectiondata(inputbox);
                        if(selection_data) {
                            SDL_SetClipboardText(selection_data);
                        }
                        break;
                    case SDLK_v:
                        char* clipboard_data = SDL_GetClipboardText();
                        if(input->selection_count) {
                            input->cursor_pos = input->selection_end;
                            if(input->selection_count == input->character_count) {
                                inputbox->render_pos = 0;
                            }
                            for(size_t i=0;i<input->selection_count;i++) {
                                mplayer_inputbox_backspace(inputbox);
                            }
                            mplayer_inputbox_clearselection(inputbox);
                        }
                        mplayer_inputbox_addinputdata(inputbox, clipboard_data);
                        free(inputbox->input.data);
                        inputbox->input.data = mplayer_inputbox_getinputdata(inputbox);
                        free(clipboard_data);
                        break;
                }
                break;
            } else if(SDL_GetModState() & KMOD_SHIFT) {
                switch(mplayer->e.key.keysym.sym) {
                    case SDLK_LEFT:
                        mplayer_inputbox_addselection(inputbox, INPUT_SELECTIONLEFT);
                        mplayer_inputbox_printselection(inputbox);
                        mplayer_inputbox_cursor_resetblink(inputbox);
                        break;
                    case SDLK_RIGHT:
                        mplayer_inputbox_addselection(inputbox, INPUT_SELECTIONRIGHT);
                        mplayer_inputbox_printselection(inputbox);
                        mplayer_inputbox_cursor_resetblink(inputbox);
                        break;
                }
                break;
            }
            switch(mplayer->e.key.keysym.sym) {
                case SDLK_LEFT:
                    mplayer_inputbox_cursor_resetblink(inputbox);
                    if(input->selection_count) {
                        input->cursor_pos = input->selection_start;
                        mplayer_inputbox_clearselection(inputbox);
                        break;
                    }
                    if(inputbox->input.cursor_pos) {
                        inputbox->input.cursor_pos--;
                    }
                    break;
                case SDLK_RIGHT:
                    mplayer_inputbox_cursor_resetblink(inputbox);
                    if(input->selection_count) {
                        input->cursor_pos = input->selection_end;
                        mplayer_inputbox_clearselection(inputbox);
                        break;
                    }
                    if(inputbox->input.cursor_pos < inputbox->input.character_count) {
                        inputbox->input.cursor_pos++;
                    }
                    break;
                case SDLK_BACKSPACE:
                    mplayer_inputbox_cursor_resetblink(inputbox);
                    if(input->selection_count) {
                        input->cursor_pos = input->selection_end;
                        if(input->selection_count == input->character_count) {
                            inputbox->render_pos = 0;
                        }
                        for(size_t i=0;i<input->selection_count;i++) {
                            mplayer_inputbox_backspace(inputbox);
                        }
                        mplayer_inputbox_clearselection(inputbox);
                        free(inputbox->input.data);
                        inputbox->input.data = mplayer_inputbox_getinputdata(inputbox);
                        break;
                    }
                    mplayer_inputbox_backspace(inputbox);
                    free(inputbox->input.data);
                    inputbox->input.data = mplayer_inputbox_getinputdata(inputbox);
                    break;
                case SDLK_RETURN:
                    inputbox->entered = true;
                    break;
            }
            break;
    }
}

void mplayer_inputbox_rendercharacters(mplayer_t* mplayer, mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    size_t end_renderpos = 0;
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y,
        *mouse_startx = &inputbox->mouse_startx, *mouse_endx = &inputbox->mouse_endx;
    for(size_t i=inputbox->render_pos;i<input->character_count;i++) {
        inputchar_t* character = &input->characters[i],
            *next_character = (i+1 < input->character_count) ? &input->characters[i+1] :
                &input->characters[i];
        SDL_Surface* char_surface = TTF_RenderUTF8_Blended(inputbox->font, character->utf8_char,
            inputbox->text_color);
        SDL_Texture* char_texture = SDL_CreateTextureFromSurface(mplayer->renderer, char_surface);
        SDL_FreeSurface(char_surface);
        if(i == inputbox->render_pos) {
            character->canvas.x = inputbox->canvas.x;
        } else {
            character->canvas.x = input->characters[i-1].canvas.x
                + input->characters[i-1].canvas.w;
        }
        character->canvas.y = inputbox->canvas.y + (inputbox->canvas.h - character->canvas.h) / 2;
        if(input->cursor_pos == i) {
            character->canvas.x += inputbox->cursor_width;
        }
        size_t previ = (i > 0) ? i-1 : i;
        
        if(mplayer->mouse_buttondown && mouse_x <= character->canvas.x + character->canvas.w
            && mouse_x >= character->canvas.x + 3
            && mouse_y <= character->canvas.y + character->canvas.h
            && mouse_y >= character->canvas.y && !inputbox->start_xinited) {
            // Set the start mouse x to determine mouse selection type
            *mouse_startx = mouse_x;
            inputbox->start_xinited = true;
        } else if(mplayer->mouse_buttondown && mouse_x <= character->canvas.x + character->canvas.w
            && mouse_x >= character->canvas.x + 3
            && mouse_y <= character->canvas.y + character->canvas.h
            && mouse_y >= character->canvas.y && !inputbox->end_xinited) {
            // Set the end mouse x to determine mouse selection type
            *mouse_endx = mouse_x;
            inputbox->end_xinited = true;
        } else if(mplayer->mouse_buttondown && mouse_x <= character->canvas.x + character->canvas.w
            && mouse_x >= character->canvas.x + 3
            && mouse_y <= character->canvas.y + character->canvas.h
            && mouse_y >= character->canvas.y && !inputbox->mouse_drag
            && inputbox->start_xinited && inputbox->end_xinited) {
            if(input->selection_count) {
                mplayer_inputbox_clearselection(inputbox);
            }
            input->cursor_pos = i;
            printf("Adding initial Selection point\n");
            if((*mouse_startx) < (*mouse_endx)) {
                mplayer_inputbox_addselection(inputbox, INPUT_SELECTIONRIGHT);
            } else {
                mplayer_inputbox_addselection(inputbox, INPUT_SELECTIONLEFT);
            }    
            mplayer_inputbox_printselection(inputbox);
            inputbox->mouse_drag = true;
        } else if(inputbox->mouse_drag && input->selection_count
            && mouse_x >= character->canvas.x + character->canvas.w &&
            input->selection_direction == INPUT_SELECTIONRIGHT) {
            printf("Adding more selections to right:\n");
            input->cursor_pos = i;
            mplayer_inputbox_addselection(inputbox, INPUT_SELECTIONRIGHT);
            mplayer_inputbox_printselection(inputbox);
        } else if(inputbox->mouse_drag && input->selection_count
            && mouse_x <= character->canvas.x + character->canvas.w
            && input->selection_direction == INPUT_SELECTIONLEFT) {
            printf("Adding more selections to left:\n");
            //input->cursor_pos = i;
            mplayer_inputbox_addselection(inputbox, INPUT_SELECTIONLEFT);
            mplayer_inputbox_printselection(inputbox);
        } else if(!mplayer->mouse_buttondown && inputbox->mouse_drag) {
            printf("setting drag state to false\n");
            inputbox->mouse_drag = false;
            mplayer->mouse_clicked = false;
        } else {
            int diffx = next_character->canvas.x - (character->canvas.x + character->canvas.w);
            if(mplayer->mouse_clicked && mplayer_rect_hover(mplayer, inputbox->canvas)
            && !inputbox->mouse_drag) {
                if(mouse_x <= character->canvas.x + character->canvas.w-1 &&
                    mouse_x >= character->canvas.x && diffx) {
                    printf("Selecting character: %s\n", character->utf8_char); 
                    input->cursor_pos = i;
                    mplayer_inputbox_clearselection(inputbox);
                    mplayer->mouse_clicked = false;
                } else if(mouse_x <= character->canvas.x + character->canvas.w &&
                    mouse_x >= character->canvas.x && !diffx) {
                    printf("Selecting character: %s\n", character->utf8_char);
                    input->cursor_pos = i+1;
                    mplayer_inputbox_clearselection(inputbox);
                    mplayer->mouse_clicked = false;
                }
            } 
        }
        end_renderpos = i+1;
        if(character->canvas.x + character->canvas.w > inputbox->canvas.x + inputbox->canvas.w) {
            break;
        }
        if(input->selection_count && i <= input->selection_end-1 && i >= input->selection_start) {
            SDL_SetRenderDrawColor(mplayer->renderer, 0x00, 0xff, 0xff, 0xff);
            SDL_RenderDrawRect(mplayer->renderer, &character->canvas);
            SDL_RenderFillRect(mplayer->renderer, &character->canvas);
        }
        SDL_RenderCopy(mplayer->renderer, char_texture, NULL, &character->canvas);
        SDL_DestroyTexture(char_texture);
    }
    if(mplayer->mouse_clicked && mplayer_rect_hover(mplayer, inputbox->canvas) &&
        mouse_x >= input->characters[end_renderpos-1].canvas.x +
        input->characters[end_renderpos-1].canvas.w) {
        input->cursor_pos = end_renderpos;
        mplayer->mouse_clicked = false;
    }
    inputbox->cursor_canvas.x = inputbox->canvas.x + 1;
    if(input->cursor_pos == inputbox->render_pos && inputbox->render_pos) {
        inputbox->render_pos--;
    } else if(input->cursor_pos >= end_renderpos-1 && input->characters && input->characters[end_renderpos-1].canvas.x + input->characters[end_renderpos-1].canvas.w >=
        inputbox->canvas.x + inputbox->canvas.w) {
        inputbox->render_pos++;
    } else if(input->characters && input->cursor_pos) {
        inputbox->cursor_canvas.x = input->characters[input->cursor_pos-1].canvas.x + input->characters[input->cursor_pos-1].canvas.w;
    }
    if(input->cursor_pos < inputbox->render_pos) {
        inputbox->render_pos--;
    }
}

void mplayer_inputbox_rendercursor(mplayer_t* mplayer,  mplayer_inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    inputbox->cursor_canvas.w = inputbox->cursor_width;
    inputbox->cursor_canvas.h = inputbox->cursor_height;
    inputbox->cursor_canvas.y = inputbox->canvas.y + (inputbox->canvas.h - inputbox->cursor_canvas.h)/2;
    if(!inputbox->show_cursor) {
        return;
    }
    if(inputbox->cursor_blink) {
        if(!inputbox->cursor_timeout) {
            inputbox->cursor_timeout = SDL_GetTicks64() + (inputbox->blink_timeoutsecs * 1000);
        } else if(inputbox->cursor_blinking) {
            if(SDL_GetTicks64() >= inputbox->cursor_timeout) {                
                inputbox->cursor_timeout = 0;
                inputbox->cursor_blinking = false;
            }
            return;
        } else if(!inputbox->cursor_blinking && SDL_GetTicks64() >= inputbox->cursor_timeout) {
            inputbox->cursor_timeout = 0;
            inputbox->cursor_blinking = true;
        }
    }
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(inputbox->cursor_color));
    SDL_RenderDrawRect(mplayer->renderer, &inputbox->cursor_canvas);
    SDL_RenderFillRect(mplayer->renderer, &inputbox->cursor_canvas);
}

void mplayer_inputbox_renderplaceholder(mplayer_t* mplayer,  mplayer_inputbox_t* inputbox) {
    if(!inputbox->placeholder_show || inputbox->input.characters || !inputbox->placeholder) {
        return;
    }
    SDL_RenderSetClipRect(mplayer->renderer, &inputbox->canvas);
    TTF_SetFontSize(inputbox->font, inputbox->font_size);
    TTF_SizeText(inputbox->font, inputbox->placeholder, &inputbox->placeholder_canvas.w,
        &inputbox->placeholder_canvas.h);
    SDL_Surface* placeholder_surface = TTF_RenderUTF8_Blended(inputbox->font,
        inputbox->placeholder, inputbox->placeholder_color);
    SDL_Texture* placeholder_texture = SDL_CreateTextureFromSurface(mplayer->renderer, placeholder_surface);
    SDL_FreeSurface(placeholder_surface);
    SDL_RenderCopy(mplayer->renderer, placeholder_texture, NULL, &inputbox->placeholder_canvas);
    SDL_DestroyTexture(placeholder_texture);
    SDL_RenderSetClipRect(mplayer->renderer, NULL);
}

void mplayer_inputbox_display(mplayer_t* mplayer,  mplayer_inputbox_t* inputbox) {
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(inputbox->canvas_color));
    SDL_RenderDrawRect(mplayer->renderer, &inputbox->canvas);
    if(inputbox->fill) {
        SDL_RenderFillRect(mplayer->renderer, &inputbox->canvas);
    }
    if(mplayer_rect_hover(mplayer, inputbox->canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_TYPEABLE);
    }
    mplayer_inputbox_renderplaceholder(mplayer, inputbox);
    mplayer_inputbox_rendercharacters(mplayer, inputbox);
    mplayer_inputbox_rendercursor(mplayer, inputbox);
}

void mplayer_inputbox_clear(mplayer_inputbox_t* inputbox) {
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        free(inputbox->input.characters[i].utf8_char);
    }
    free(inputbox->input.characters); inputbox->input.characters = NULL;
    free(inputbox->input.data); inputbox->input.data = NULL;
    mplayer_inputbox_clearselection(inputbox);
    inputbox->render_pos = 0;
    inputbox->input.size = 0;
    inputbox->input.cursor_pos = 0;
    inputbox->input.character_count = 0;
}

bool mplayer_inputbox_hover(mplayer_t* mplayer,  mplayer_inputbox_t* inputbox) {
    if(mplayer_rect_hover(mplayer, inputbox->canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_TYPEABLE);
        return true;
    }
    return false;
}

bool mplayer_inputbox_clicked(mplayer_t* mplayer,  mplayer_inputbox_t* inputbox) {
    if(mplayer->mouse_clicked) {
        if(mplayer_inputbox_hover(mplayer, inputbox)) {
            inputbox->clicked = true;
            mplayer->mouse_clicked = false;
            return true;
        }
        inputbox->clicked = false;
    }
    return false;
}

void mplayer_inputbox_destroy(mplayer_inputbox_t* inputbox) {
    mplayer_inputbox_clear(inputbox);
    free(inputbox->placeholder);
    inputbox->placeholder = NULL;
}