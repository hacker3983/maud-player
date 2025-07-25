#ifndef _MAUD_PLAYQUEUE_H
#define _MAUD_PLAYQUEUE_H
#include "maud_player.h"
#include "maud_textmanager.h"

void maud_queue_init(maud_queue_t* queue);
maud_queueitem_t* maud_queue_realloclist_by(maud_queue_t* queue, size_t amount);
size_t maud_queue_getmusic_count(maud_queue_t queue);
void maud_queue_swap(size_t* a, size_t* b);
void maud_queue_swapitem(maud_queueitem_t* a, maud_queueitem_t* b);
bool maud_queue_splitby_playid(maud_queue_t* queue, size_t playindex, size_t playid);
void maud_queue_handleplaybutton(maud_t* maud, maud_queue_t* queue, size_t item_index);
void maud_queue_handleitem_selection(maud_t* maud, maud_queue_t* queue, size_t item_index,
    SDL_Rect outer_canvas, text_info_t* item_textinfo);
void maud_queue_handlecheckbox_itemselection(maud_t* maud, maud_queue_t* queue, size_t item_index);
bool maud_queue_addmusic(maud_queue_t* queue, size_t uid, size_t music_listindex, size_t music_id);
bool maud_queue_addmusicqueue_toplaynext(maud_t* maud, maud_queue_t* destination_queue, maud_queue_t* source_queue);
bool maud_queue_addmusicfrom_queue(maud_queue_t* destination_queue, maud_queue_t* source_queue);
void maud_queue_removemusicby_uid(maud_queue_t* queue, size_t uid);
void maud_queue_removemusicby_playid(maud_queue_t* queue, size_t playid);
void maud_queue_removemusicby_musiclistidx_id(maud_queue_t* queue, size_t music_listindex, size_t musicid);
void maud_queue_display(maud_t* maud, maud_queue_t* queue);
void maud_queue_print(maud_t* maud, maud_queue_t queue);
void maud_queue_destroy(maud_queue_t* queue);
#endif