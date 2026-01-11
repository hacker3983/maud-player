#include "maud_selectionmenu.h"

void maud_selectionmenu_create(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_renderer_display(maud);
}

void maud_selectionmenu_handle_selectallbtn_toggleoption(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    maud_selectionmenubtn_t* addto_btn = &selection_menu->addtobtn;
    bool *fill = &select_allbtn->checkbox.fill,
         *tick = &select_allbtn->checkbox.tick;
    if(addto_btn->clicked) {
        return;
    }
    size_t target_count = (active_tab == SONGS_TAB) ? maud->music_count
                        : (active_tab == QUEUES_TAB) ? maud->play_queue.item_count : 0;
    if(maud_rect_hover(maud, select_allbtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        *fill = true;
        if(maud->mouse_clicked) {
            if(*tick) {
                *fill = false, *tick = false;
            } else {
                *tick = true;
            }
            select_allbtn->clicked = true;
            maud->checkall_btntoggled = true;
            maud->mouse_clicked = false;
        }
    } else if(!select_allbtn->clicked && maud->tick_count == target_count) {
        *fill = true, *tick = true;
    } else {
        *fill = false, *tick = false;
        select_allbtn->clicked = false;
    }
}

void maud_selectionmenu_handle_playbtn(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* playbtn = &selection_menu->playbtn;
    maud_queue_t *play_queue = &maud->play_queue,
                 *selection_queue = &maud->selection_queue;
    if(!maud_rect_hover(maud, playbtn->canvas)) {
        return;
    }
    size_t music_listindex = 0, music_id = 0, *playid = &play_queue->playid;
    maud_setcursor(maud, MAUD_CURSOR_POINTER);
    if(maud->mouse_clicked) {
        printf("You clicked the play button now we are gonna modify the play queue\n");
        if(active_tab == QUEUES_TAB) {
            *playid = selection_queue->items[0].uid;
        } else {
            maud_queue_destroy(play_queue);
            if(!maud_queue_addmusicfrom_queue(play_queue, selection_queue)) {
                printf("Failed to add the selection queue to the play queue\n");
                Mix_HaltMusic();
                return;
            }
            printf("Successfully added the selection queue to the play queue and play queue now looks like:");
            printf("maud->play_queue->item_count = %zu\n", play_queue->item_count);
            maud_queue_print(maud, *play_queue);
        }
        if(Mix_PlayingMusic() ) {
            Mix_PauseMusic();
            Mix_HaltMusic();
        }
        music_listindex = play_queue->items[*playid].music_listindex,
        music_id = play_queue->items[*playid].music_id;
        if(play_queue->items && !Mix_PlayMusic(maud->music_lists[music_listindex]
            [music_id].music, 1)) {
            printf("Playing music %s\n",
                maud->music_lists[music_listindex][music_id].music_name
            );
        }
        maud_selectionmenu_clearmusic_selection(maud, selection_menu);
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_handle_playnextbtn(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* playnextbtn = &selection_menu->playnextbtn;
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    if(maud_rect_hover(maud, playnextbtn->canvas)) {
        if(maud->mouse_clicked) {
            bool playqueue_wasempty = !maud->play_queue.items;
            maud_queue_addmusicqueue_toplaynext(maud, &maud->play_queue, &maud->selection_queue);
            if(!Mix_PlayingMusic() && playqueue_wasempty) {
                maud->play_queue.playid = 0;
                size_t playid = 0, music_listindex = maud->play_queue.items[playid].music_listindex,
                       music_id = maud->play_queue.items[playid].music_id;
                Mix_PlayMusic(maud->music_lists[music_listindex][music_id].music, 1);
            }
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
            select_allbtn->toggled = true;
        }
    }
}

void maud_selectionmenu_clearmusic_selection(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    for(size_t i=0;i<maud->play_queue.item_count;i++) {
        maud->play_queue.items[i].fill = false;
        maud->play_queue.items[i].checkbox_ticked = false;
    }
    if(active_tab == QUEUES_TAB) {
        maud->songsbox_resized = true;
    }
    for(size_t i=0;i<maud->selection_queue.item_count;i++) {
        size_t music_listindex = maud->selection_queue.items[i].music_listindex,
        music_id = maud->selection_queue.items[i].music_id;
        maud->music_lists[music_listindex][music_id].fill = false;
        maud->music_lists[music_listindex][music_id].checkbox_ticked = false;
    }

    maud->tick_count = 0;
    select_allbtn->checkbox.tick = false;
    select_allbtn->checkbox.fill = false;
    select_allbtn->clicked = false;
    free(maud->selection_queue.items); maud->selection_queue.items = NULL;
    maud->selection_queue.item_count = 0;
}

void maud_selectionmenu_display(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_renderer_display(maud);
}