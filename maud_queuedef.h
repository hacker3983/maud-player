#ifndef _MUSIC_QUEUEDEF_H
#define _MUSIC_QUEUEDEF_H
#include <stddef.h>

typedef struct music_queueitem {
    size_t uid, music_listindex, music_id;
    bool fill, checkbox_ticked;
} music_queueitem_t;

typedef struct music_queue {
    music_queueitem_t* items;
    size_t playid, item_count;
} music_queue_t;
#endif