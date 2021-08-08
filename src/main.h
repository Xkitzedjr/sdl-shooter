#include "common.h"

extern void cleanup(void);
extern void doInput(void);
extern void initSDL(void);
extern void initStage(void);
extern void prepareScene(void);
extern void presentScene(void);
extern void initSounds(void);
extern void initFonts(void);
extern void initHighscore(void);
extern void initBackground(void);
extern void initStarefield(void);
extern void initSounds(void);
extern void initHighscoreTable(void);
extern void loadMusic(char *filename);
extern void playMusic(int repeat);
extern void initGame(void);
extern void initTitle(void);
extern void initDB(void);

App app;
Stage stage;
Highscores highscores;

static void capFrameRate(long *then, float *remainder);
