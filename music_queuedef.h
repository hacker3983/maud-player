#ifndef _MUSIC_QUEUEDEF_H
#define _MUSIC_QUEUEDEF_H
#include <stddef.h>
typedef struct music_queueitem {
    size_t music_listindex;
    size_t* music_ids;
    size_t music_count;
} music_queueitem_t;

typedef struct music_queue {
    music_queueitem_t* items;
    size_t item_count, play_itemindex, playid,
        totalmusic_count;
} music_queue_t;
#endif