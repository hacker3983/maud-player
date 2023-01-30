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
    for(size_t i=0;i<mplayer.musinfo.file_count;i++) {
        printf("%s\n", mplayer.musinfo.files[i].path);
    }
    mplayer_run(&mplayer);
    mplayer_destroyall(&mplayer);
    return 0;
}