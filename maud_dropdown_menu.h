#include "maud_dropdown_menudef.h"
#include "maud_string.h"
#include "maud_player.h"

void maud_dropdown_menu_init(maud_dropdown_menu_t* menu, int x, int y,
    int width, int height, SDL_Color color, int textspace_x, int textspace_y,
    int iconspace_x, int iconspace_y, int icon_width, int icon_height);
bool maud_dropdown_menu_add(maud_dropdown_menu_t* menu, TTF_Font* font,
    const char* item_name, int font_size, const char* icon_path,
    SDL_Texture* icon_texture, SDL_Color text_color, SDL_Color background_color);
bool maud_dropdown_menu_remove(maud_dropdown_menu_t* menu, const char* item_name);
void maud_dropdown_menu_render(maud_t* maud, maud_dropdown_menu_t* menu);
void maud_dropdown_menu_print(maud_dropdown_menu_t* menu);
void maud_dropdown_menu_destroy(maud_dropdown_menu_t* menu);