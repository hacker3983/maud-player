#include "maud_settingsmenu.h"
#include "maud_colorpicker.h"

void maud_settingmenu_rendercustomizetab_headertext(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    text_info_t header_text = {
        .font_size = 36,
        .text = "Customize",
        .text_canvas = {
            .x = navbar->canvas.x + navbar->canvas.w + 10
        },
        .text_color = {0xFF, 0xFF, 0x00, 0xFF},
    };
    maud_textmanager_sizetext(maud->font, &header_text);
    SDL_Texture* customize_texture = maud_textmanager_rendertext(maud, maud->font,
        &header_text);
    SDL_RenderCopy(maud->renderer, customize_texture, NULL,
        &header_text.text_canvas);
    SDL_DestroyTexture(customize_texture);
    customize_tab->header_text = header_text;
}

void maud_settingmenu_rendercustomizetab_colorpicker(maud_t* maud,
    maud_customizetab_t* customize_tab) {
    text_info_t header_text = customize_tab->header_text;
    maud_colorpicker_t* color_picker = &customize_tab->color_picker;
    maud_colorpicker_setpreview_position(
        color_picker,
        header_text.text_canvas.x,
        header_text.text_canvas.y + header_text.text_canvas.h + 20
    );
    maud_colorpicker_setsliders_trackposition(
        color_picker,
        color_picker->preview_canvas.x,
        color_picker->preview_canvas.y +
        color_picker->preview_canvas.h + 10,
        20
    );
    maud_colorpicker_display(maud, color_picker);
}

void maud_settingmenu_displaycustomize_tab(maud_t* maud) {
    maud_settingmenu_navbar_t* navbar = &maud->setting_navbar;
    maud_customizetab_t* customize_tab = &navbar->customize_tab;
    maud_settingmenu_rendercustomizetab_headertext(maud, customize_tab);
    maud_settingmenu_rendercustomizetab_colorpicker(maud, customize_tab);
}