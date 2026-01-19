#include "maud_dropdown_menu.h"
#include "maud_textmanager.h"
#include "maud_playlistmanager.h"

void maud_dropdown_menu_init(maud_dropdown_menu_t* menu, int x, int y,
    int width, int height, SDL_Color color, int textspace_x, int textspace_y,
    int iconspace_x, int iconspace_y, int icon_width, int icon_height) {
    SDL_Rect* canvas = &menu->canvas;
    canvas->x = x, canvas->y = y,
    canvas->w = width, canvas->h = height;

    menu->color = color;

    menu->textspace_x = textspace_x,
    menu->textspace_y = textspace_y;

    menu->iconspace_x = iconspace_x,
    menu->iconspace_y = iconspace_y;

    menu->icon_width = icon_width,
    menu->icon_height = icon_height;
}

bool maud_dropdown_menu_add(maud_dropdown_menu_t* menu, TTF_Font* font,
    const char* item_name, int font_size, const char* icon_path,
    SDL_Texture* icon_texture, SDL_Color text_color, SDL_Color background_color) {
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
    new_items[new_itemcount-1].font = font;
    new_items[new_itemcount-1].color = background_color;
    new_items[new_itemcount-1].name.font_size = font_size;
    new_items[new_itemcount-1].name.utext = item_namedup;
    new_items[new_itemcount-1].name.text_color = text_color;
    new_items[new_itemcount-1].icon_canvas.w = menu->icon_width,
    new_items[new_itemcount-1].icon_canvas.h = menu->icon_height;
    maud_textmanager_sizetext(font, &new_items[new_itemcount-1].name);
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

void maud_dropdown_menu_render(maud_t* maud, maud_dropdown_menu_t* menu) {
    SDL_Rect *canvas = &menu->canvas;
    int start_y = canvas->y;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(menu->color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);
    for(size_t i=0;i<menu->item_count;i++) {
        maud_dropdown_item_t* items = menu->items;
        SDL_Rect *icon_canvas = &items[i].icon_canvas,
                 *text_canvas = &items[i].name.text_canvas;

        icon_canvas->x = canvas->x + menu->iconspace_x;
        icon_canvas->y = start_y + menu->iconspace_y;
        if(items[i].icon_path && !items[i].icon_texture) {
            items[i].icon_texture = IMG_LoadTexture(maud->renderer, items[i].icon_path);
        }
        SDL_Texture* icon_texture = items[i].icon_texture;
        SDL_RenderCopy(maud->renderer, icon_texture, NULL, &items[i].icon_canvas);

        text_canvas->x = icon_canvas->x + icon_canvas->w + menu->textspace_x;
        text_canvas->y = icon_canvas->y;
        SDL_Texture* text_texture = maud_textmanager_renderunicode(maud, items[i].font,
            &items[i].name);
        SDL_RenderCopy(maud->renderer, text_texture, NULL, &items[i].name.text_canvas);

        start_y = icon_canvas->y + icon_canvas->h + menu->iconspace_y;
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