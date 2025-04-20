#include "music_notification.h"
#include "music_textmanager.h"
#include "music_string.h"

void mplayer_notification_push(music_notification_t* notification, TTF_Font* font, int font_size, SDL_Color background_color,
    const char* message, SDL_Color message_color, double timeout_secs, int padding_x, int padding_y,
    int message_spacing) {
    size_t message_len = strlen(message);
    char* message_dup = mplayer_dupstr(message, message_len);
    if(!message_dup) {
        return;
    }
    music_notificationitem_t* new_items = NULL;
    size_t new_itemcount = 0;
    if(!notification->items) {
        new_items = malloc(sizeof(music_notificationitem_t));
        new_itemcount = 1;
    } else {
        new_items = realloc(notification->items, (notification->item_count + 1) *
            sizeof(music_notificationitem_t));
        new_itemcount = notification->item_count + 1;
    }
    if(!new_items) {
        free(message_dup);
        message_dup = NULL;
        return;
    }
    new_items[new_itemcount-1].font = font;
    new_items[new_itemcount-1].background_color = background_color;
    new_items[new_itemcount-1].message = message_dup;
    new_items[new_itemcount-1].message_info.font_size = font_size;
    new_items[new_itemcount-1].message_info.text = NULL;
    new_items[new_itemcount-1].message_info.utext = message_dup;
    new_items[new_itemcount-1].message_info.text_color = message_color;
    new_items[new_itemcount-1].message_segments = NULL;
    new_items[new_itemcount-1].message_segmentcount = 0;
    new_items[new_itemcount-1].message_spacing = message_spacing;
    new_items[new_itemcount-1].padding_x = padding_x;
    new_items[new_itemcount-1].padding_y = padding_y;
    new_items[new_itemcount-1].timeout = SDL_GetTicks64() + (timeout_secs * 1000);
    new_items[new_itemcount-1].timeout_secs = timeout_secs;
    notification->items = new_items;
    notification->item_count = new_itemcount;
    printf("Successfully pushed notification: %s\n", message);
}

void mplayer_notification_pop(music_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    if(SDL_GetTicks64() >= notification->items[0].timeout) {
        printf("Successfully popped notification: %s\n", notification->items[0].message);
        free(notification->items[0].message);
        for(size_t i=0;i<notification->item_count-1;i++) {
            music_notificationitem_t current_item = notification->items[i];
            notification->items[i] = notification->items[i+1];
            notification->items[i+1] = current_item;
        }
        music_notificationitem_t* new_items = realloc(notification->items, (notification->item_count-1)
            * sizeof(music_notificationitem_t));
        // Update the timeout of the next notification    
        if(new_items) {
            new_items[0].timeout = SDL_GetTicks64() + (new_items[0].timeout_secs * 1000);
        }
        notification->items = new_items;
        notification->item_count = notification->item_count-1;
    }
}

bool mplayer_notification_addmsgsegment(music_notification_t* notification, text_info_t message_segment) {
    if(!notification->items) {
        return false;
    }
    music_notificationitem_t* item = &notification->items[0];
    size_t new_segmentcount = item->message_segmentcount+1;
    text_info_t* new_segments = realloc(item->message_segments, new_segmentcount * sizeof(text_info_t));
    if(!new_segments) {
        return false;
    }
    new_segments[new_segmentcount-1] = message_segment;
    item->message_segments = new_segments;
    item->message_segmentcount = new_segmentcount;
    return true;
}

void mplayer_notification_getsize(music_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    int max_textw = 500;
    music_notificationitem_t *item = &notification->items[0];
    size_t message_len = strlen(item->message);
    text_info_t message_segment = item->message_info;
    char* new_segment = NULL;
    int total_segmentw = 0, total_segmenth = 0, max_segmentw = 0;
    for(size_t i=0;i<message_len;i++) {
        char* utf8_char = mplayer_getutf8_char(item->message, &i, message_len);
        mplayer_concatstr(&new_segment, utf8_char);
        message_segment.utext = new_segment;
        mplayer_textmanager_sizetext(item->font, &message_segment);
        total_segmentw = message_segment.text_canvas.w;
        if(total_segmentw > max_segmentw) {
            max_segmentw = total_segmentw;
        }
        if(total_segmentw >= max_textw) {
            total_segmenth += message_segment.text_canvas.h;
            if(!mplayer_notification_addmsgsegment(notification, message_segment)) {
                free(new_segment);
                new_segment = NULL;
                break;
            }
            new_segment = NULL;
        }
        free(utf8_char);
    }
    if(new_segment) {
        mplayer_notification_addmsgsegment(notification, message_segment);
        total_segmenth += message_segment.text_canvas.h;
    }
    item->canvas.w = max_segmentw + item->padding_x;
    item->canvas.h = total_segmenth + item->padding_y + (item->message_spacing *
        (item->message_segmentcount-1));
}

void mplayer_notification_clearsegment(music_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    for(size_t i=0;i<notification->items[0].message_segmentcount;i++) {
        free(notification->items[0].message_segments[i].utext);
    }
    free(notification->items[0].message_segments);
    notification->items[0].message_segments = NULL;
    notification->items[0].message_segmentcount = 0;
}

void mplayer_notification_display(mplayer_t* mplayer, music_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    music_notificationitem_t *item = &notification->items[0];
    TTF_Font* font = item->font;
    text_info_t* message_info = NULL;
    SDL_Rect* canvas = &item->canvas;
    mplayer_notification_getsize(notification);
    canvas->x = mplayer->win_width - canvas->w - 10;
    canvas->y = mplayer->win_height - music_status.h - canvas->h - 10;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(item->background_color));
    SDL_RenderDrawRect(mplayer->renderer, canvas);
    SDL_RenderFillRect(mplayer->renderer, canvas);
    for(size_t i=0;i<item->message_segmentcount;i++) {
        int message_spacing = (i > 0 && i != item->message_segmentcount-1) ? 0 : item->message_spacing;
        int prev_y = (i > 0) ? item->message_segments[i-1].text_canvas.y +
            item->message_segments[i-1].text_canvas.h + message_spacing : canvas->y + (item->padding_y/2);
        message_info = &item->message_segments[i];
        message_info->text_canvas.x = canvas->x + (canvas->w - message_info->text_canvas.w) / 2;
        message_info->text_canvas.y = prev_y;

        SDL_Texture* message_texture = mplayer_textmanager_renderunicode(mplayer, font, message_info);
        SDL_RenderCopy(mplayer->renderer, message_texture, NULL, &message_info->text_canvas);
        SDL_DestroyTexture(message_texture); message_texture = NULL;
    }
    mplayer_notification_clearsegment(notification);
    mplayer_notification_pop(notification);
}

void mplayer_notification_destroy(music_notification_t* notification) {
    for(size_t i=0;i<notification->item_count;i++) {
        free(notification->items[i].message);
        for(size_t j=0;j<notification->items[i].message_segmentcount;j++) {
            free(notification->items[i].message_segments[j].utext);
        }
        free(notification->items[i].message_segments);
    }
    free(notification->items);
    notification->items = NULL;
    notification->item_count = 0;
}