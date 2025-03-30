#include "music_notification.h"
#include "music_textmanager.h"
#include "music_string.h"

void mplayer_notification_push(music_notification_t* notification, TTF_Font* font, int font_size, SDL_Color background_color,
    const char* message, SDL_Color message_color, double timeout_secs, int padding_x, int padding_y) {
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
    new_items[new_itemcount-1].padding_x = padding_x;
    new_items[new_itemcount-1].padding_y = padding_y;
    uint64_t previous_timeout = (new_itemcount > 1) ?
        new_items[new_itemcount-2].timeout - new_items[new_itemcount-2].timestamp : 0,
        timestamp = SDL_GetTicks64() + previous_timeout + 10;
    new_items[new_itemcount-1].timestamp = timestamp;
    new_items[new_itemcount-1].timeout = timestamp + (timeout_secs * 1000);
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
        notification->items = new_items;
        notification->item_count = notification->item_count-1;
    }
}

void mplayer_notification_display(mplayer_t* mplayer, music_notification_t* notification) {
    if(!notification->items) {
        return;
    }
    TTF_Font* font = notification->items[0].font;
    text_info_t* message_info = &notification->items[0].message_info;
    mplayer_textmanager_sizetext(font, message_info);
    SDL_Rect* canvas = &notification->items[0].canvas;
    canvas->w = message_info->text_canvas.w + notification->items[0].padding_x;
    canvas->h = message_info->text_canvas.h + notification->items[0].padding_y;
    canvas->x = WIDTH - canvas->w - 10;
    canvas->y = HEIGHT - music_status.h - canvas->h - 10;

    message_info->text_canvas.x = canvas->x + (canvas->w - message_info->text_canvas.w) / 2;
    message_info->text_canvas.y = canvas->y + (canvas->h - message_info->text_canvas.h) / 2;

    if(SDL_GetTicks64() >= notification->items[0].timestamp) {
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(notification->items[0].background_color));
        SDL_RenderDrawRect(mplayer->renderer, canvas);
        SDL_RenderFillRect(mplayer->renderer, canvas);

        SDL_Texture* message_texture = mplayer_textmanager_renderunicode(mplayer, font, message_info);
        SDL_RenderCopy(mplayer->renderer, message_texture, NULL, &message_info->text_canvas);
        SDL_DestroyTexture(message_texture); message_texture = NULL;
    }
    mplayer_notification_pop(notification);
}

void mplayer_notification_destroy(music_notification_t* notification) {
    for(size_t i=0;i<notification->item_count;i++) {
        free(notification->items[i].message);
    }
    free(notification->items);
    notification->items = NULL;
    notification->item_count = 0;
}
