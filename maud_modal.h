#include "maud_player.h"

void maud_modal_init(maud_modal_t* modal);
maud_modal_t maud_modal_create();
void maud_modal_getsize(maud_t* maud, maud_modal_t* modal);
void maud_modal_setposition(maud_t* maud, maud_modal_t* modal);
void maud_modal_display(maud_t* maud, maud_modal_t* modal);
void maud_modal_handle_inputevents(maud_t* maud, maud_modal_t* modal);
void maud_modal_handle_events(maud_t* maud, maud_modal_t* modal);
void maud_modal_destroy(maud_modal_t* modal);