#include "music_queue.h"
#include "music_scrollcontainer.h"
#include "music_checkboxes.h"
#include "music_songsmanager.h"

void mplayer_queue_init(music_queue_t* queue) {
    queue->items = NULL;
    queue->item_count = 0;
    queue->play_itemindex = 0;
    queue->playid = 0;
    queue->totalmusic_count = 0;
}

music_queueitem_t* mplayer_queue_realloclist_by(music_queue_t* queue, size_t amount) {
    music_queueitem_t* new_queueitems = NULL;
    if(!amount) {
        return queue->items;
    }
    if(!queue->items) {
        new_queueitems = malloc(amount * sizeof(music_queueitem_t));
        if(!new_queueitems) {
            return NULL;
        }
        queue->item_count = amount;
        memset(new_queueitems, 0, amount * sizeof(music_queueitem_t));
    } else {
        size_t old_size = queue->item_count;
        new_queueitems = realloc(queue->items, (queue->item_count + amount)
            * sizeof(music_queueitem_t));
        if(!new_queueitems) {
            return NULL;
        }
        queue->item_count += amount;
        memset(new_queueitems+queue->item_count, 0, (queue->item_count - old_size) *
            sizeof(music_queueitem_t));
    }
    queue->items = new_queueitems;
    return new_queueitems;
}

size_t* mplayer_queue_reallocmusicid_listby(size_t** music_ids, size_t amount, size_t* music_count) {
    size_t* new_musicids = NULL;
    if(amount == 0) {
        return *music_ids;
    }
    if(!(*music_ids)) {
        new_musicids = malloc(amount * sizeof(size_t));
        *music_count = 0;
    } else {
        new_musicids = realloc(*music_ids, ((*music_count)+amount) * sizeof(size_t));
    }
    if(new_musicids) {
        *music_ids = new_musicids;
        (*music_count) += amount;
    }
    return new_musicids;
}

bool mplayer_queue_addmusic(music_queue_t* queue, size_t music_listindex, size_t music_id) {
    music_queueitem_t* new_musiclists = NULL;
    if(!(queue->items) || (queue->items[queue->item_count-1].music_listindex
        != music_listindex)) {
        if(!mplayer_queue_realloclist_by(queue, 1)) {
            fprintf(stderr, "mplayer_queue_realloclistby failed\n");
            return false;
        }
    }
    size_t item_index = queue->item_count-1,
           music_idindex = 0;
    queue->totalmusic_count++;
    if(!mplayer_queue_reallocmusicid_listby(&queue->items[item_index].music_ids, 1,
        &queue->items[item_index].music_count)) {
        return false;
    }
    music_idindex = queue->items[item_index].music_count-1;
    queue->items[item_index].music_listindex = music_listindex;
    queue->items[item_index].music_ids[music_idindex] = music_id;
    return true;
}

bool mplayer_queue_addmusicfrom_queue(music_queue_t* destination_queue, music_queue_t* source_queue) {
    if(!source_queue->items) {
        return false;
    } 
    music_queueitem_t* new_items = NULL;
    size_t old_itemcount = destination_queue->item_count;
    if(!destination_queue->items || destination_queue->items[old_itemcount-1].music_listindex !=
        source_queue->items[0].music_listindex) {
        new_items = mplayer_queue_realloclist_by(destination_queue, source_queue->item_count);
        if(!new_items) {
            printf("Failed to allocate memory for addmusicfrom queue function\n");
            return false;
        }
        for(size_t i=old_itemcount, j=0;i<destination_queue->item_count;i++, j++) {
            mplayer_queue_reallocmusicid_listby(&new_items[i].music_ids, source_queue->items[j].music_count,
                &new_items[i].music_count);
            for(size_t k=0;k<new_items[i].music_count;k++) {
                new_items[i].music_ids[k] = source_queue->items[j].music_ids[k];
            }
            //new_items[i] = source_queue->items[j];
            destination_queue->totalmusic_count += source_queue->items[j].music_count;
        }
        destination_queue->items = new_items;
        return true;
    }
    // Allocate memory for the new music ids since the music_listindex is the same thing between the
    // destination and selection queue and copy the structures appropriately
    size_t oldmusic_count = destination_queue->items[old_itemcount-1].music_count,
        *new_musicids = mplayer_queue_reallocmusicid_listby(
            &destination_queue->items[old_itemcount-1].music_ids,
            source_queue->items[0].music_count,
            &destination_queue->items[old_itemcount-1].music_count
        );
    if(!new_musicids) {
        printf("Failed to allocate memory for new_musicids\n");
        return false;
    }
    // Add the first source queue item to the last destination queue item
    for(size_t i=0;i<source_queue->items[0].music_count;i++) {
        new_musicids[oldmusic_count+i] = source_queue->items[0].music_ids[i];
    }
    destination_queue->items[old_itemcount-1].music_ids = new_musicids;
    destination_queue->totalmusic_count += source_queue->items[0].music_count;
    // Whenever the source queue item count is greater than one then we reallocate memory to store
    // the additional items and then copy the additional items
    if(source_queue->item_count > 1) {
        new_items = mplayer_queue_realloclist_by(destination_queue,
            source_queue->item_count-1);
        if(!new_items) {
            return false;
        }
        destination_queue->items = new_items;
        for(size_t i=1;i<source_queue->item_count;i++) {
            mplayer_queue_reallocmusicid_listby(&new_items[old_itemcount+(i-1)].music_ids,
                source_queue->items[i].music_count, &new_items[i].music_count);
            for(size_t j=0;j<new_items[old_itemcount+(i-1)].music_count;j++) {
                new_items[old_itemcount].music_ids[j] = source_queue->items[i].music_ids[j];
            }
            destination_queue->totalmusic_count += source_queue->items[i].music_count;
        }
    }
    return true;
}

void mplayer_queue_removemusicby_id(music_queue_t* queue, size_t music_listindex, size_t music_id) {
    if(!queue->item_count) {
        return;
    }
    bool found = false;
    music_queue_t new_queue = {0};
    for(size_t i=0;i<queue->item_count;i++) {
        if(queue->items[i].music_listindex != music_listindex) {
            continue;
        }
        for(size_t j=0;j<queue->items[i].music_count;j++) {
            if(queue->items[i].music_ids[j] == music_id && !found) {
                found = true; continue;
            }
            mplayer_queue_addmusic(&new_queue, queue->items[i].music_listindex,
                queue->items[i].music_ids[j]);
        }
    }
    mplayer_queue_destroy(queue);
    queue->items = new_queue.items;
    queue->item_count = new_queue.item_count;
}

void mplayer_queue_swap(size_t* a, size_t* b) {
    size_t temp = *a;
    *a = *b, *b = temp;
}

void mplayer_queue_swapitem(music_queueitem_t* a, music_queueitem_t* b) {
    music_queueitem_t temp = *a;
    *a = *b, *b = temp;
}

bool mplayer_queue_splitby_playid(music_queue_t* queue, size_t playindex, size_t playid) {
    if(!queue->items) {
        return false;
    }
    music_queueitem_t* item = &queue->items[playindex], *new_items = NULL,
    *new_item = NULL;
    size_t newmusic_count = item->music_count - (playid+1);
    // if there are not any playids or music ids to split after current playid then return true
    if(!newmusic_count) {
        return true;
    }

    // Increase the queue item count by 1
    if(!(new_items = mplayer_queue_realloclist_by(queue, 1))) {
        return false;
    }
    // Get the pointer to th new item so we can get the play ids that come after the current one
    new_item = &new_items[queue->item_count-1];
    if(!mplayer_queue_reallocmusicid_listby(&new_item->music_ids, newmusic_count,
        &new_item->music_count)) {
        queue->items = realloc(queue->items, (queue->item_count-1) * sizeof(music_queueitem_t));
        queue->item_count--;
        return false;
    }
    // set the music list index appropriately before splitting
    new_item->music_listindex = item->music_listindex;
    // Initialize the music ids that come after the current playid
    for(size_t i=0,j=playid+1;j<item->music_count;i++,j++) {
        new_item->music_ids[i] = item->music_ids[j];
    }
    // Deallocate memory for the music ids that come after the current play id
    item->music_count = playid+1;
    item->music_ids = realloc(item->music_ids, item->music_count * sizeof(size_t));
    // Shift the new item from the end of the queue to come after the current playid since it was splitted
    for(size_t j=queue->item_count-1;j>playindex+1;j--) {
        music_queueitem_t current_item = new_items[j],
                          prev_item = new_items[j-1];
        new_items[j] = prev_item, new_items[j-1] = current_item;
    }
    return true;
}

bool mplayer_queue_addmusicqueue_toplaynext(mplayer_t* mplayer, music_queue_t* destination_queue, music_queue_t* source_queue) {
    size_t play_itemindex = destination_queue->play_itemindex,
           playid = destination_queue->playid;
    // If the destination queue / play queue is empty then we just add music and return true from our function
    if(!destination_queue->items) {
        if(!mplayer_queue_addmusicfrom_queue(destination_queue, source_queue)) {
            return false;
        }
        return true;    
    }
    // If the destination queue or play queue is not empty then we get the current play item
    music_queueitem_t* curritem = &destination_queue->items[play_itemindex];
    // we check to see if the current item music list index is the same as the first item in our source queue
    // if that condition is true then we realloc the music id list for that particular queue item and
    // add the music ids in that source queue to go after the playid
    if(curritem->music_listindex == source_queue->items[0].music_listindex) {
        if(!mplayer_queue_reallocmusicid_listby(&curritem->music_ids, source_queue->items[0].music_count,
            &curritem->music_count)) {
            return false;
        }
        for(size_t i=0;i<source_queue->items[0].music_count;i++) {
            curritem->music_ids[curritem->music_count-1] = source_queue->items[0].music_ids[i];
            for(size_t j=curritem->music_count-1;j>playid+1;j--) {
                mplayer_queue_swap(&curritem->music_ids[j], &curritem->music_ids[j-1]);
            }
            playid++;
        }
        // Deallocate memory for the first item of music ids since we are not copying
        // its pointer directly from the source queue
        free(source_queue->items[0].music_ids);
        size_t old_itemcount = destination_queue->item_count;
        destination_queue->totalmusic_count += source_queue->items[0].music_count;
        // Whenever the source queue item count is less than two we just return true from the function
        if(source_queue->item_count < 2) {
            return true;
        }
        // if the their are more items then we just reallocate our destination queue by the rest of the
        // number of items that need to be added
        if(!mplayer_queue_realloclist_by(destination_queue, source_queue->item_count-1)) {
            return false;
        }
        // after we have done that we place the rest of the items in the queue
        for(size_t i=1;i<source_queue->item_count;i++) {
            destination_queue->items[old_itemcount - (i+1)] = source_queue->items[i];
            destination_queue->totalmusic_count += source_queue->items[i].music_count;
        }
        // we then shift the rest of the items from the end of our queue to come after our current
        // play id such that those music play after the current music is finished
        for(size_t i=0;i<source_queue->item_count-1;i++) {
            for(size_t j=destination_queue->item_count-1;j>play_itemindex+1;j--) {
                mplayer_queue_swapitem(&destination_queue->items[j], &destination_queue->items[j-1]);
            }
            play_itemindex++;
        }
        return true;
    }
    // whenever the first item in our source queue and current play item are not equal
    // we try to split our queue item so that the items after the current playid are in a separate item
    if(!mplayer_queue_splitby_playid(destination_queue, play_itemindex, playid)) {
        return false;
    }
    // we then add the musics from the source queue to the destination queue
    if(!mplayer_queue_addmusicfrom_queue(destination_queue, source_queue)) {
        return false;
    }
    // we then shift the added items so that they come after the current play id or the current music
    // that is being played
    for(size_t i=0;i<source_queue->item_count;i++) {
        for(size_t j=destination_queue->item_count-1;j>play_itemindex+1;j--) {
            mplayer_queue_swapitem(&destination_queue->items[j], &destination_queue->items[j-1]);
        }
        play_itemindex++;
    }
    return true;
}

size_t mplayer_queue_getmusic_count(music_queue_t queue) {
    return queue.totalmusic_count;
}

void mplayer_queue_display(mplayer_t* mplayer, music_queue_t queue) {
    music_scrollcontainer_t* queue_scrollcontainer = &mplayer->play_queuescrollcontainer;
    text_info_t music_name = {
        .font_size = 18,
        .text = NULL,
        .text_canvas = {
            .x = 0, .y = 0,
            .w = 0, .h = 0
        },
        .text_color = white,
        .utext = NULL
    };
    SDL_Rect scroll_area = {
        .x = songs_box.x + 2, .y = songs_box.y + 1,
        .w = WIDTH, .h = songs_box.h
    };
    SDL_RenderSetClipRect(mplayer->renderer, &scroll_area);
    mplayer_scrollcontainer_setprops(queue_scrollcontainer, scroll_area, 20, 0, queue.totalmusic_count);
    SDL_Rect outer_canvas = {
        .x = songs_box.x + 2, .y = queue_scrollcontainer->scroll_y,
        .w = WIDTH - scrollbar.w - 5, .h = 0
    };
    SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
        fill_color = {0xFF, 0xA5, 0x00, 0xff};
    size_t content_renderpos = 0;;
    for(size_t i=0;i<queue.item_count;i++) {
        size_t music_listindex = queue.items[i].music_listindex;
        music_t* music_list = mplayer->music_lists[music_listindex];
        for(size_t j=0;j<queue.items[i].music_count;j++) {
            if(content_renderpos < queue_scrollcontainer->content_renderpos) {
                content_renderpos++;
                continue;
            }
            size_t music_id = queue.items[i].music_ids[j];
            music_name.utext = mplayer->music_lists[music_listindex][music_id].music_name;
            mplayer_textmanager_sizetext(mplayer->font, &music_name);
            outer_canvas.h = music_name.text_canvas.h + 22;
            music_name.text_canvas.x = outer_canvas.x + 50;
            music_name.text_canvas.y = outer_canvas.y + (outer_canvas.h - music_name.text_canvas.h)/2;
            checkbox_size.x = outer_canvas.x+5;
            checkbox_size.h = outer_canvas.h-10;
            checkbox_size.y = outer_canvas.y + ((outer_canvas.h - checkbox_size.h)/2);
            if(outer_canvas.y < scroll_area.y + scroll_area.h) {
                mplayer_scrollcontainer_additem(queue_scrollcontainer, outer_canvas);
                mplayer_scrollcontainer_setnext_itemcanvas(queue_scrollcontainer, outer_canvas);
            } else {
                break;
            }
            if(!mplayer->tick_count) {
                music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = outer_canvas.h - 10;
                music_listplaybtn.btn_canvas.x = (checkbox_size.x + checkbox_size.w) + 20,
                music_listplaybtn.btn_canvas.y = checkbox_size.y;
            }
            SDL_Texture* music_nametexture = mplayer_textmanager_renderunicode(mplayer,
                mplayer->music_font, &music_name);
            SDL_SetRenderDrawColor(mplayer->renderer, 0, 42, 50, 0xFF /*0x3B, 0x35, 0x61, 0xFF*/);
            SDL_RenderDrawRect(mplayer->renderer, &outer_canvas);
            SDL_RenderFillRect(mplayer->renderer, &outer_canvas);
            
            if(mplayer_rect_hover(mplayer, outer_canvas)) {
                mplayer_songsmanager_handlesong_playbutton_hover(mplayer, outer_canvas);
                if(mplayer_rect_hover(mplayer, music_listplaybtn.btn_canvas)) {
                    mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                }
                if(mplayer_checkbox_hovered(mplayer)) {
                    mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                }
                if(!mplayer->tick_count) {
                    music_name.text_canvas.x += music_listplaybtn.btn_canvas.w + 20;
                    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
                        [music_listplaybtn.texture_idx], NULL, &music_listplaybtn.btn_canvas);
                }
            }
            if(mplayer->tick_count) {
                // whenever any checkbox is ticked and the current music we are current music we are rendering is not ticked
                // we ensure that we set the checkbox fill state to false before drawing its checkbox
                if(!music_list[music_id].checkbox_ticked && !mplayer_rect_hover(mplayer, outer_canvas)) {
                    music_list[music_id].fill = false;
                }
                mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, music_list[music_id].fill, tick_color,
                    music_list[music_id].checkbox_ticked);
            } else if(mplayer_rect_hover(mplayer, outer_canvas)) {
                // whenever no checkbox is ticked and we hover over the music we display the check box
                // so that the user can click it or select it
                if(mplayer_checkbox_hovered(mplayer)) {
                    mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, music_list[music_id].fill, tick_color,
                        music_list[music_id].checkbox_ticked);
                } else if(!mplayer_checkbox_hovered(mplayer)) {
                    mplayer_drawmusic_checkbox(mplayer, box_color, fill_color, false, tick_color, false);
                }
            }
            SDL_RenderCopy(mplayer->renderer, music_nametexture, NULL, &music_name.text_canvas);
            SDL_DestroyTexture(music_nametexture); music_nametexture = NULL;
            outer_canvas.y += outer_canvas.h + 3;
        }
    }
    // scroll bar related information
    mplayer_scrollbar_t songsbox_scrollbar = {
        .rect = scrollbar,
        .displacement = 0.0,
        .orientation = 0,
        .start_pos = queue_scrollcontainer->content_renderpos + 1,
        .final_pos = mplayer_queue_getmusic_count(queue),
        .padding_x = -2,
        .padding_y = -(2 + (int)((double)scrollbar.h / 2.0)),
        .scroll_area = scroll_area
    };
    SDL_RenderSetClipRect(mplayer->renderer, NULL);
    mplayer_renderscroll_bar(mplayer, &songsbox_scrollbar, 8);
    if(mplayer->scroll) {
        mplayer_scrollcontainer_performscroll(mplayer, queue_scrollcontainer);
        mplayer->scroll = false;
    }
    if(queue_scrollcontainer->item_container.items) {
        mplayer_scrollcontainer_init(queue_scrollcontainer);
    }
    mplayer_scrollcontainer_resetitem_index(queue_scrollcontainer);
}

void mplayer_queue_print(mplayer_t* mplayer, music_queue_t queue) {
    printf("[\n");
    size_t item_count = queue.item_count;
    for(size_t i=0;i<item_count;i++) {
        size_t music_count = queue.items[i].music_count;
        for(size_t j=0;j<music_count;j++) {
            size_t music_listindex = queue.items[i].music_listindex,
                   music_id = queue.items[i].music_ids[j];
            char* music_name = mplayer->music_lists[music_listindex][music_id].music_name;
            printf("\t%zu:{music id:%d, music_name: %s}", i+1, music_id, music_name);
            if(j != music_count-1) {
                printf(",\n");
            }
        }
        if(i != item_count-1) {
            printf(",\n");
        }
    }
    printf("\n]\n");
}

void mplayer_queue_destroy(music_queue_t* queue) {
    for(size_t i=0;i<queue->item_count;i++) {
        free(queue->items[i].music_ids); queue->items[i].music_ids = NULL;
    }
    free(queue->items); queue->items = NULL;
    mplayer_queue_init(queue);
}