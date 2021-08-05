#ifndef BACKGROUND_H_
#define BACKGROUND_H_

#include "common.h"

static int backgroundX;
static Star stars[MAX_STARS];
static SDL_Texture *background;

extern SDL_Texture *loadTexture(char *filename);

extern App app;

#endif // BACKGROUND_H_
