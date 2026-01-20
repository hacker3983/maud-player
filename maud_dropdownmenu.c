#include "maud_dropdown_menu.h"
#include "maud_textmanager.h"
#include "maud_playlistmanager.h"

void maud_dropdown_menu_init(maud_dropdown_menu_t* menu, int x, int y,
    int width, int height, SDL_Color color, int itemspace_x, int itemspace_y,
    int textspace_x, int icon_width, int icon_height) {
    SDL_Rect* canvas = &menu->canvas;
    canvas->x = x, canvas->y = y,
    canvas->w = width, canvas->h = height;

    menu->color = color;

    menu->itemspace_x = itemspace_x;
    menu->itemspace_y = itemspace_y;
    menu->textspace_x = textspace_x,

    menu->icon_width = icon_width,
    menu->icon_height = icon_height;
}

bool maud_dropdown_menu_add(maud_dropdown_menu_t* menu, TTF_Font* font,
    const char* item_name, int font_size, const char* icon_path,
    SDL_Texture* icon_texture, SDL_Color text_color) {
    char* item_namedup = maud_dupstr(item_name, strlen(item_name));
    if(!item_namedup) {
        return false;
    }
    size_t new_itemcount = menu->item_count + 1;
    maud_dropdown_item_t* new_items = realloc(menu->items,
        new_itemcount * sizeof(maud_dropdown_item_t));
    if(!new_items) {
        free(item_namedup);
        return false;
    }
    memset(new_items+menu->item_count, 0, sizeof(maud_dropdown_item_t));
    SDL_Rect *text_canvas = &new_items[new_itemcount-1].name.text_canvas,
             *icon_canvas = &new_items[new_itemcount-1].icon_canvas,
             *canvas = &new_items[new_itemcount-1].canvas;
    new_items[new_itemcount-1].font = font;
    new_items[new_itemcount-1].name.font_size = font_size;
    new_items[new_itemcount-1].name.utext = item_namedup;
    new_items[new_itemcount-1].name.text_color = text_color;
    maud_textmanager_sizetext(font, &new_items[new_itemcount-1].name);
    icon_canvas->w = menu->icon_width,
    icon_canvas->h = menu->icon_height;
    canvas->w = icon_canvas->w + menu->textspace_x
        + text_canvas->w;
    canvas->h = maud_playlistmanager_findmaxheight((SDL_Rect[]) {
        *icon_canvas,
        *text_canvas
    }, 2);
    if(icon_path) {
        new_items[new_itemcount-1].icon_path = maud_dupstr(icon_path, strlen(icon_path));
    } else if(icon_texture) {
        new_items[new_itemcount-1].icon_texture = icon_texture;
    }
    menu->items = new_items;
    menu->item_count = new_itemcount;
    return true;
}

bool maud_dropdown_menu_remove(maud_dropdown_menu_t* menu, const char* item_name) {
    if(!menu->items) {
        return false;
    }
    bool found = false;
    for(size_t i=0;i<menu->item_count;i++) {
        if(!found && strcmp(menu->items[i].name.utext, item_name) == 0) {
            found = true;
        }
        if(found && i < menu->item_count-1) {
            maud_dropdown_item_t next_item = menu->items[i+1];
            menu->items[i+1] = menu->items[i];
            menu->items[i] = next_item;
        }
    }
    if(found) {
        menu->item_count--;
        free(menu->items[menu->item_count].name.utext);
        if(menu->items[menu->item_count].icon_path) {
            free(menu->items[menu->item_count].icon_path);
            free(menu->items[menu->item_count].icon_texture);
        }
        menu->items = realloc(menu->items, menu->item_count *
            sizeof(maud_dropdown_item_t));
    }
    return found;
}

void maud_dropdown_menu_handleitem_event(maud_t* maud, maud_dropdown_menu_t* menu, size_t i) {
    maud_dropdown_item_t* item = &menu->items[i];
    if(maud_rect_hover(maud, item->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("You clicked the %s button within the drop down menu\n",
                item->name.utext);
            item->clicked = true;
            menu->item_id = i;
            menu->item_clicked = true;
            maud->mouse_clicked = false;
        }
    }
}

void maud_dropdown_menu_setitem_positions(maud_t* maud, maud_dropdown_menu_t* menu) {
    SDL_Rect* canvas = &menu->canvas;
    int start_y = canvas->y;
    for(size_t i=0;i<menu->item_count;i++) {
        maud_dropdown_item_t* items = menu->items;
        SDL_Rect *item_canvas = &items[i].canvas,
                 *icon_canvas = &items[i].icon_canvas,
                 *text_canvas = &items[i].name.text_canvas;
        item_canvas->x = canvas->x + menu->itemspace_x;
        item_canvas->y = start_y + menu->itemspace_y;
        maud_dropdown_menu_handleitem_event(maud, menu, i);

        icon_canvas->x = item_canvas->x;
        icon_canvas->y = item_canvas->y + (item_canvas->h -
            icon_canvas->h) / 2;

        text_canvas->x = icon_canvas->x + icon_canvas->w + menu->textspace_x;
        text_canvas->y = item_canvas->y + (item_canvas->h -
            text_canvas->h) / 2;
        start_y = item_canvas->y + item_canvas->h + menu->itemspace_y;
    }
}

void maud_dropdown_menu_render(maud_t* maud, maud_dropdown_menu_t* menu) {
    SDL_Rect* canvas = &menu->canvas;
    int start_y = canvas->y;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(menu->color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);
    for(size_t i=0;i<menu->item_count;i++) {
        maud_dropdown_item_t* items = menu->items;
        SDL_Rect *icon_canvas = &items[i].icon_canvas,
                 *text_canvas = &items[i].name.text_canvas;        
        if(items[i].icon_path && !items[i].icon_texture) {
            items[i].icon_texture = IMG_LoadTexture(maud->renderer, items[i].icon_path);
        }
        SDL_Texture* icon_texture = items[i].icon_texture;
        SDL_RenderCopy(maud->renderer, icon_texture, NULL, icon_canvas);

        SDL_Texture* text_texture = maud_textmanager_renderunicode(maud, items[i].font,
            &items[i].name);
        SDL_RenderCopy(maud->renderer, text_texture, NULL, text_canvas);
    }
}

void maud_dropdown_menu_print(maud_dropdown_menu_t* menu) {
    printf("Drop down items are:\n");
    for(size_t i=0;i<menu->item_count;i++) {
        printf("item[%zu] = %s\n", i, menu->items[i].name.utext);
    }
}

void maud_dropdown_menu_destroy(maud_dropdown_menu_t* menu) {
    for(size_t i=0;i<menu->item_count;i++) {
        free(menu->items[i].name.utext);
        if(menu->items[i].icon_path) {
            free(menu->items[i].icon_path);
            free(menu->items[i].icon_texture);
        }
    }
    free(menu->items); menu->items = NULL;
    menu->item_count = 0;
}