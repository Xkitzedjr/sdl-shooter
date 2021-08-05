#include "common.h"

extern SDL_Texture *loadTexture(char *filename);
extern void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y);

static SDL_Texture *fontTexture;
static char drawTextBuffer[MAX_LINE_LENGTH];
