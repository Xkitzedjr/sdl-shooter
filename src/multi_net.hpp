#ifndef MULTI_NET_H_
#define MULTI_NET_H_

#include "common.h"
#include "common_multiplayer.hpp"
#include <iostream>
#include <array>
#include <list>
#include <memory>

template <class T> using UP = std::unique_ptr<T>;

extern App app;

extern SDL_Texture *loadTexture(char *filename);
extern void doBackground(void);
extern void doStarfield(void);
extern void drawBackground(void);
extern void drawStarfield(void);
extern void playSound(int id, int channel);
extern void blit(SDL_Texture *texture, int x, int y);
extern void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void drawText(int x, int y, int r, int g, int b, int align, char *format, ...);
extern void updateToBeSent(int x, int y, OP_CODE opCode);
extern void getLastRecieved(UP<Message> &returnMessage);
extern void killConnection(void);
extern void initHighscore(void);

struct M_Stage {
	std::unique_ptr<Entity> player1;
	std::unique_ptr<Entity> player2;
	std::list<UP<Entity>> bullets;
	std::list<UP<Explosion>> explosions;
	std::list<UP<Debris>> debris;

    M_Stage() {}

    ~M_Stage() {
        killConnection();
    }
};

static M_Stage stage;
static SDL_Texture *playerTexture;
static SDL_Texture *p1bulletTexture;
static SDL_Texture *player2Texture;
static SDL_Texture *p2BulletTexture;
static SDL_Texture *background;
static SDL_Texture *explosionTexture;
static int stageResetTime;
static int p1score, p2score;

static void logic(void);
static void doPlayer(void);
static void doPlayer2(void);
static void fireBullet(void);
static void p2fireBullet(void);
static void doBullets(void);
static void draw(void);
static void drawBullets(void);
static void initPlayer(void);
static void initPlayer2(void);
static void doFighters(void);
static void drawFighters(void);
static void resetStage(void);
static void clipPlayer(void);
static void doExplosion(void);
static void doDebris(void);
static void addExplosions(int x, int y, int num);
static void addDebris(UP<Entity>& e);
static void drawDebris(void);
static void drawExplosions(void);
static void drawHud(void);
static int bulletHitFighter(UP<Entity>& e);

#endif // MULTI_H_
