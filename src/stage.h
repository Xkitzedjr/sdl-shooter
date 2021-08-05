#include "common.h"

extern App app;
extern Stage stage;
extern Highscores highscores;

extern void addHighscore(int score);
extern void blit(SDL_Texture *texture, int x, int y);
extern SDL_Texture *loadTexture(char *filename);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
extern void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y);
extern void loadMusic(char *filename);
extern void playMusic(int loop);
extern void playSound(int id, int channel);
extern void drawText(int x, int y, int r, int g, int b, int align,char *format, ...);
extern void initHighscore(void);
extern void addHighScore(int score);
extern void doBackground(void);
extern void doStarfield(void);
extern void drawBackground(void);
extern void drawStarfield(void);

static Entity *player;
static SDL_Texture *bulletTexture, *enemyTexture, *alienBulletTexture, *playerTexture;
static SDL_Texture *background, *explosionTexture, *pointsTexture;
static int enemySpawnTimer;
static int stageResetTime;
static Star stars[MAX_STARS];
static int score;
static int highScore;

static void initPlayer(void);
static void doPlayer(void);
static void fireBullet(void);
static void doBullets(void);
static void draw(void);
static void drawPlayer(void);
static void drawBullets(void);
static void logic(void);
static void drawFighters(void);
static void spawnEnemies(void);
static void doFighters(void);
static int bulletHitFighter(Entity *b);
static void resetStage(void);
static void clipPlayer(void);
static void doEnemies(void);
static void fireAlienBullet(Entity *e);
static void initStarfield(void);
static void doExplosion(void);
static void doDebris(void);
static void drawDebris(void);
static void drawExplosions(void);
static void addExplosions(int x, int y, int num);
static void addDebris(Entity *e);
static void drawHud(void);
static void doPointsPod(void);
static void addPointsPod(int x, int y);
static void drawPointsPod(void);
