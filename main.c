#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "maud_player.h"

int main(int argc, char** argv) {
    maud_t maud = {0};
    maud_init();
    maud_createapp(&maud);
    maud_run(&maud);
    maud_destroyapp(&maud);
    return 0;
}