#ifndef libplayer_h
#define libplayer_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* PlayerCtx;


/**
    Create Lib Player Context
    This method preprare Video Pipeline
    The next step is to call libplayer_set_media();

    @return PlayerCtx Player Opaque Context or NULL if there is an error
 */
PlayerCtx libplayer_init();


/**
    The next step is to call libplayer_start();
    @param PlayerCtx Player Opaque Context or NULL if there is an error
 */
void libplayer_set_media(PlayerCtx ctx, char* uri);

void libplayer_start(PlayerCtx ctx, void* view);
void libplayer_play(PlayerCtx ctx);
void libplayer_pause(PlayerCtx ctx);

/**
    Release libplayer_resources
    @param ctx : LibStudio Context
 */
void libplayer_stop(PlayerCtx ctx);

#ifdef __cplusplus
}
#endif

#endif /* libplayer_h */