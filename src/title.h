#ifndef TITLE_H_
#define TITLE_H_

#include "common.h"

static int timeout, reveal;
static SDL_Texture *sdl2Texture;
static SDL_Texture *shooterTexture;

static void drawLogo(void);
static void logic(void);
static void draw(void);

extern App app;

extern void doBackground(void);
extern void doStarfield(void);
extern SDL_Texture *loadTexture(char *filename);
extern void initHighscore(void);
extern void initStage(void);
extern void drawBackground(void);
extern void drawStarfield(void);
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void blitRect(SDL_Texture *texture, SDL_Rect *r, int x, int y);
extern void initMulti(void);
extern void initConnection();

#endif // TITLE_H_
