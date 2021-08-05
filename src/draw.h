#include "common.h"
#include "SDL2/SDL_image.h"

static SDL_Texture *getTexture(char *name);
static void addTextureToCache(char *name, SDL_Texture *sdlTexture);

extern App app;
