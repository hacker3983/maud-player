#include "maud_playerscroll_types.h"

int maud_scrolltype_getmousewheel_scrolltype(SDL_Event e, int* scroll_typeptr) {
    int scroll_type = -1;
    if(e.wheel.y > 0) {
        scroll_type = MAUDSCROLL_UP;
        printf("SCROLL_UP\n");
    } else if(e.wheel.y < 0) {
        scroll_type = MAUDSCROLL_DOWN;
        printf("SCROLL_DOWN\n");
    } else if(e.wheel.x > 0) {
        scroll_type = MAUDSCROLL_RIGHT;
        printf("SCROLL_RIGHT\n");
    } else if(e.wheel.x < 0) {
        scroll_type = MAUDSCROLL_LEFT;
        printf("SCROLL_LEFT\n");
    }
    // Check that the scroll_typeptr is not null before setting it to the scroll type
    // and ensure that the scroll_type is not in deceleration of zero
    if(scroll_typeptr && scroll_type != -1) {
        *scroll_typeptr = scroll_type;
    }
    return scroll_type;
}