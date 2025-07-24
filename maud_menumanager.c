#include "music_menumanager.h"

void mplayer_menumanager_setup_menu(mplayer_t* mplayer) {
    bool menu_initialized = false;
    mplayer_menu_t* menu = &mplayer->menus[mplayer->menu_opt];
    mplayer->menu = menu;
    // If the menu is already initialized then we won't reinitialize it
    if(menu->textures[MPLAYER_TEXT_TEXTURE] || menu->textures[MPLAYER_BUTTON_TEXTURE]
        || menu->textures[MPLAYER_TAB_TEXTURE]) {
        return;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
        mplayer_menumanager_createmenu_texture(mplayer, MPLAYER_TEXT_TEXTURE, text_info_size-1);
        mplayer_menumanager_createmenu_texture(mplayer, MPLAYER_TAB_TEXTURE, tab_info_size);
        mplayer_menumanager_createmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE, MUSICBTN_COUNT/*MTOTALBTN_COUNT*/);
        // Load Button Textures and Canvas's
        for(int i=0;i<MUSICBTN_COUNT;i++) {
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, music_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
            music_btns[i].texture_idx = i;
        }
        
        // Add a texture for the add folder icon button
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_addfolderbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_addfolderbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] = music_addfolderbtn.btn_canvas;

        // Add a texture for settings icon button
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        setting_iconbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            setting_iconbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] = setting_iconbtn.btn_canvas;

        // music list play button icon
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_listplaybtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_listplaybtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx] = music_listplaybtn.btn_canvas;

        // Add music to playlist button
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_addplaylistbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_addplaylistbtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_addplaylistbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addplaylistbtn.texture_idx] = music_addplaylistbtn.btn_canvas;

        // Add add to button used for adding music to play queue or new playlist as long as they are selected
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_addtobtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_addtobtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_addtobtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addtobtn.texture_idx] = music_addtobtn.btn_canvas;

        // Add the circle add to button to the texture list
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_addtocirclebtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_addtocirclebtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_addtocirclebtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addtocirclebtn.texture_idx] = music_addtocirclebtn.
            btn_canvas;

        // Add play queue btn used for adding music to play queue
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_playqueuebtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_playqueuebtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_playqueuebtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_playqueuebtn.texture_idx] = music_playqueuebtn.btn_canvas;

        // Add playlist btn to the texture list
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_playlistbtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_playlistbtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_playlistbtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_playlistbtn.texture_idx] = music_playlistbtn.btn_canvas;
    } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU) {
        mplayer_menumanager_createmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE, SETTINGSBTN_COUNT);
        for(size_t i=0;i<SETTINGSBTN_COUNT;i++) {
            menu->textures[MPLAYER_BUTTON_TEXTURE][i] = IMG_LoadTexture(mplayer->renderer, setting_btns[i].imgbtn_path);
            menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = setting_btns[i].btn_canvas;
            setting_btns[i].texture_idx = i;
        }

        // music location remove button
        mplayer_menumanager_addmenu_texture(mplayer, MPLAYER_BUTTON_TEXTURE);
        music_removebtn.texture_idx = menu->texture_sizes[MPLAYER_BUTTON_TEXTURE]-1;
        menu->textures[MPLAYER_BUTTON_TEXTURE][music_removebtn.texture_idx] = IMG_LoadTexture(mplayer->renderer,
            music_removebtn.imgbtn_path);
        menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_removebtn.texture_idx] = music_removebtn.btn_canvas;
    }
}

void mplayer_menumanager_createmenu_texture(mplayer_t* mplayer, int texture_type, size_t amount) {
    mplayer->menu->textures[texture_type] = mplayer_texturemanager_createtexture_list(amount);
    mplayer->menu->texture_canvases[texture_type] = calloc(amount, sizeof(SDL_Rect));
    mplayer->menu->texture_sizes[texture_type] = amount;
}

void mplayer_menumanager_addmenu_texture(mplayer_t* mplayer, int texture_type) {
    size_t *amount = &mplayer->menu->texture_sizes[texture_type];
    mplayer_texturemanager_realloctexture(&mplayer->menu->textures[texture_type],
       amount);
    mplayer->menu->texture_canvases[texture_type] = realloc(mplayer->menu->texture_canvases[texture_type],
        (*amount) * sizeof(SDL_Rect));
    memset(mplayer->menu->texture_canvases[texture_type], 0, (*amount) * sizeof(SDL_Rect));
}

void mplayer_menumanager_menuplace_texture(mplayer_t* mplayer, int type, SDL_Texture* texture, SDL_Rect canvas) {
    size_t texture_size = mplayer->menu->texture_sizes[type];
    mplayer->menu->textures[type][texture_size-1] = texture;
    mplayer->menu->texture_canvases[type][texture_size-1] = canvas;
}


void mplayer_menumanager_menuadd_canvas(mplayer_t* mplayer, SDL_Rect canvas) {
    size_t* canvas_count = &mplayer->menu->canvas_count;
    if(canvas_count == 0) {
        mplayer->menu->canvases = calloc(1, sizeof(SDL_Rect));
        mplayer->menu->canvases[(*canvas_count)++] = canvas;
        return;
    }
    mplayer->menu->canvases = realloc(mplayer->menu->canvases, ((*canvas_count)+1) * sizeof(SDL_Rect));
    mplayer->menu->canvases[*canvas_count] = canvas;
    (*canvas_count)++;
}

void mplayer_menumanager_menu_appendtext(mplayer_t* mplayer, text_info_t text) {
    if(!mplayer->menu->texts) {
        mplayer->menu->texts = calloc(1, sizeof(text_info_t));
        mplayer->menu->texts[0] = text;
        mplayer->menu->text_count++;
        return;
    }
    mplayer->menu->text_count++;
    mplayer->menu->texts = realloc(mplayer->menu->texts,
        (mplayer->menu->text_count) * sizeof(text_info_t));
    mplayer->menu->texts[mplayer->menu->text_count-1] = text;
}

void mplayer_menumanager_menu_freetext(mplayer_t* mplayer, int menu_option) {
    free(mplayer->menus[menu_option].texts);
    mplayer->menus[menu_option].texts = NULL;
    mplayer->menus[menu_option].text_count = 0;
}