#include "maud_menumanager.h"

void maud_menumanager_setup_menu(maud_t* maud) {
    bool menu_initialized = false;
    maud_menu_t* menu = &maud->menus[maud->menu_opt];
    maud->menu = menu;
    // If the menu is already initialized then we won't reinitialize it
    if(menu->textures[MAUD_TEXT_TEXTURE] || menu->textures[MAUD_BUTTON_TEXTURE]
        || menu->textures[MAUD_TAB_TEXTURE]) {
        return;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    if(maud->menu_opt == MAUD_DEFAULT_MENU) {
        maud_menumanager_createmenu_texture(maud, MAUD_TEXT_TEXTURE, text_info_size-1);
        maud_menumanager_createmenu_texture(maud, MAUD_TAB_TEXTURE, tab_info_size);
        maud_menumanager_createmenu_texture(maud, MAUD_BUTTON_TEXTURE, MUSICBTN_COUNT/*MTOTALBTN_COUNT*/);
        // Load Button Textures and Canvas's
        for(int i=0;i<MUSICBTN_COUNT;i++) {
            menu->textures[MAUD_BUTTON_TEXTURE][i] = IMG_LoadTexture(maud->renderer, music_btns[i].imgbtn_path);
            menu->texture_canvases[MAUD_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
            music_btns[i].texture_idx = i;
        }
        
        // Add a texture for the add folder icon button
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_addfolderbtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_addfolderbtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] = music_addfolderbtn.btn_canvas;

        // Add a texture for settings icon button
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        setting_iconbtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][setting_iconbtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            setting_iconbtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][setting_iconbtn.texture_idx] = setting_iconbtn.btn_canvas;

        // music list play button icon
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_listplaybtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_listplaybtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_listplaybtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_listplaybtn.texture_idx] = music_listplaybtn.btn_canvas;

        // Add music to playlist button
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_addplaylistbtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_addplaylistbtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_addplaylistbtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_addplaylistbtn.texture_idx] = music_addplaylistbtn.btn_canvas;

        // Add add to button used for adding music to play queue or new playlist as long as they are selected
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_addtobtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_addtobtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_addtobtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_addtobtn.texture_idx] = music_addtobtn.btn_canvas;

        // Add the circle add to button to the texture list
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_addtocirclebtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_addtocirclebtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_addtocirclebtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_addtocirclebtn.texture_idx] = music_addtocirclebtn.
            btn_canvas;

        // Add play queue btn used for adding music to play queue
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_playqueuebtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_playqueuebtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_playqueuebtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_playqueuebtn.texture_idx] = music_playqueuebtn.btn_canvas;

        // Add playlist btn to the texture list
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_playlistbtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_playlistbtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_playlistbtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_playlistbtn.texture_idx] = music_playlistbtn.btn_canvas;
    } else if(maud->menu_opt == MAUD_SETTINGS_MENU) {
        maud_menumanager_createmenu_texture(maud, MAUD_BUTTON_TEXTURE, SETTINGSBTN_COUNT);
        for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
            menu->textures[MAUD_BUTTON_TEXTURE][i] = IMG_LoadTexture(maud->renderer, setting_btns[i].imgbtn_path);
            menu->texture_canvases[MAUD_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
            setting_btns[i].texture_idx = i;
        }

        // music location remove button
        maud_menumanager_addmenu_texture(maud, MAUD_BUTTON_TEXTURE);
        music_removebtn.texture_idx = menu->texture_sizes[MAUD_BUTTON_TEXTURE]-1;
        menu->textures[MAUD_BUTTON_TEXTURE][music_removebtn.texture_idx] = IMG_LoadTexture(maud->renderer,
            music_removebtn.imgbtn_path);
        menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_removebtn.texture_idx] = music_removebtn.btn_canvas;
    }
}

void maud_menumanager_createmenu_texture(maud_t* maud, int texture_type, size_t amount) {
    maud->menu->textures[texture_type] = maud_texturemanager_createtexture_list(amount);
    maud->menu->texture_canvases[texture_type] = calloc(amount, sizeof(SDL_Rect));
    maud->menu->texture_sizes[texture_type] = amount;
}

void maud_menumanager_addmenu_texture(maud_t* maud, int texture_type) {
    size_t *amount = &maud->menu->texture_sizes[texture_type];
    maud_texturemanager_realloctexture(&maud->menu->textures[texture_type],
       amount);
    maud->menu->texture_canvases[texture_type] = realloc(maud->menu->texture_canvases[texture_type],
        (*amount) * sizeof(SDL_Rect));
    memset(maud->menu->texture_canvases[texture_type], 0, (*amount) * sizeof(SDL_Rect));
}

void maud_menumanager_menuplace_texture(maud_t* maud, int type, SDL_Texture* texture, SDL_Rect canvas) {
    size_t texture_size = maud->menu->texture_sizes[type];
    maud->menu->textures[type][texture_size-1] = texture;
    maud->menu->texture_canvases[type][texture_size-1] = canvas;
}


void maud_menumanager_menuadd_canvas(maud_t* maud, SDL_Rect canvas) {
    size_t* canvas_count = &maud->menu->canvas_count;
    if(canvas_count == 0) {
        maud->menu->canvases = calloc(1, sizeof(SDL_Rect));
        maud->menu->canvases[(*canvas_count)++] = canvas;
        return;
    }
    maud->menu->canvases = realloc(maud->menu->canvases, ((*canvas_count)+1) * sizeof(SDL_Rect));
    maud->menu->canvases[*canvas_count] = canvas;
    (*canvas_count)++;
}

void maud_menumanager_menu_appendtext(maud_t* maud, text_info_t text) {
    if(!maud->menu->texts) {
        maud->menu->texts = calloc(1, sizeof(text_info_t));
        maud->menu->texts[0] = text;
        maud->menu->text_count++;
        return;
    }
    maud->menu->text_count++;
    maud->menu->texts = realloc(maud->menu->texts,
        (maud->menu->text_count) * sizeof(text_info_t));
    maud->menu->texts[maud->menu->text_count-1] = text;
}

void maud_menumanager_menu_freetext(maud_t* maud, int menu_option) {
    free(maud->menus[menu_option].texts);
    maud->menus[menu_option].texts = NULL;
    maud->menus[menu_option].text_count = 0;
}