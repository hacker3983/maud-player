#ifndef _MAUD_QUEUEDEF_H
#define _MAUD_QUEUEDEF_H
#include <stddef.h>

typedef struct maud_queueitem {
    size_t uid, music_listindex, music_id;
    bool fill, checkbox_ticked;
} maud_queueitem_t;

typedef struct maud_queue {
    maud_queueitem_t* items;
    size_t playid, item_count;
} maud_queue_t;
#endif