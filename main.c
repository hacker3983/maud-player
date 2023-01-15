#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "music_playerinfo.h"
#include "music_player.h"

int main(int argc, char** argv) {
    mplayer_t mplayer;
    mplayer_init();
    mplayer_createall(&mplayer);
    mplayer_run(&mplayer);
    mplayer_destroyall(&mplayer);
    return 0;
}