#include <libplayer.h>

int main(){
    PlayerCtx ctx = libplayer_init();
    libplayer_start(ctx, NULL);
}