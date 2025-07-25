#include "maud_notification.h"
#include "maud_textmanager.h"
#include "maud_string.h"

void maud_notification_push(maud_notification_t* notification, TTF_Font* font, int font_size, SDL_Color background_color,
    const char* message, SDL_Color message_color, double timeout_secs, int padding_x, int padding_y,
    int message_spacing) {
    size_t message_len = strlen(message);
    char* message_dup = maud_dupstr(message, message_len);
    if(!message_dup) {
        return;
    }
    maud_notificationitem_t* new_items = NULL;
    size_t new_itemcount = 0;
    if(!notification->items) {
        new_items = malloc(sizeof(maud_notificationitem_t));
        new_itemcount = 1;
    } else {
        new_items = realloc(notification->items, (notification->item_count + 1) *
            sizeof(maud_notificationitem_t));
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

void maud_notification_pop(maud_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    if(SDL_GetTicks64() >= notification->items[0].timeout) {
        printf("Successfully popped notification: %s\n", notification->items[0].message);
        free(notification->items[0].message);
        for(size_t i=0;i<notification->item_count-1;i++) {
            maud_notificationitem_t current_item = notification->items[i];
            notification->items[i] = notification->items[i+1];
            notification->items[i+1] = current_item;
        }
        maud_notificationitem_t* new_items = realloc(notification->items, (notification->item_count-1)
            * sizeof(maud_notificationitem_t));
        // Update the timeout of the next notification    
        if(new_items) {
            new_items[0].timeout = SDL_GetTicks64() + (new_items[0].timeout_secs * 1000);
        }
        notification->items = new_items;
        notification->item_count = notification->item_count-1;
    }
}

bool maud_notification_addmsgsegment(maud_notification_t* notification, text_info_t message_segment) {
    if(!notification->items) {
        return false;
    }
    maud_notificationitem_t* item = &notification->items[0];
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

void maud_notification_getsize(maud_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    int max_textw = 500;
    maud_notificationitem_t *item = &notification->items[0];
    size_t message_len = strlen(item->message);
    text_info_t message_segment = item->message_info;
    char* new_segment = NULL;
    int total_segmentw = 0, total_segmenth = 0, max_segmentw = 0;
    for(size_t i=0;i<message_len;i++) {
        char* utf8_char = maud_getutf8_char(item->message, &i, message_len);
        maud_concatstr(&new_segment, utf8_char);
        message_segment.utext = new_segment;
        maud_textmanager_sizetext(item->font, &message_segment);
        total_segmentw = message_segment.text_canvas.w;
        if(total_segmentw > max_segmentw) {
            max_segmentw = total_segmentw;
        }
        if(total_segmentw >= max_textw) {
            total_segmenth += message_segment.text_canvas.h;
            if(!maud_notification_addmsgsegment(notification, message_segment)) {
                free(new_segment);
                new_segment = NULL;
                break;
            }
            new_segment = NULL;
        }
        free(utf8_char);
    }
    if(new_segment) {
        maud_notification_addmsgsegment(notification, message_segment);
        total_segmenth += message_segment.text_canvas.h;
    }
    item->canvas.w = max_segmentw + item->padding_x;
    item->canvas.h = total_segmenth + item->padding_y + (item->message_spacing *
        (item->message_segmentcount-1));
}

void maud_notification_clearsegment(maud_notification_t* notification) {
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

void maud_notification_display(maud_t* maud, maud_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    maud_notificationitem_t *item = &notification->items[0];
    TTF_Font* font = item->font;
    text_info_t* message_info = NULL;
    SDL_Rect* canvas = &item->canvas;
    maud_notification_getsize(notification);
    canvas->x = maud->win_width - canvas->w - 10;
    canvas->y = maud->win_height - music_status.h - canvas->h - 10;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(item->background_color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);
    for(size_t i=0;i<item->message_segmentcount;i++) {
        int message_spacing = (i > 0 && i != item->message_segmentcount-1) ? 0 : item->message_spacing;
        int prev_y = (i > 0) ? item->message_segments[i-1].text_canvas.y +
            item->message_segments[i-1].text_canvas.h + message_spacing : canvas->y + (item->padding_y/2);
        message_info = &item->message_segments[i];
        message_info->text_canvas.x = canvas->x + (canvas->w - message_info->text_canvas.w) / 2;
        message_info->text_canvas.y = prev_y;

        SDL_Texture* message_texture = maud_textmanager_renderunicode(maud, font, message_info);
        SDL_RenderCopy(maud->renderer, message_texture, NULL, &message_info->text_canvas);
        SDL_DestroyTexture(message_texture); message_texture = NULL;
    }
    maud_notification_clearsegment(notification);
    maud_notification_pop(notification);
}

void maud_notification_destroy(maud_notification_t* notification) {
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