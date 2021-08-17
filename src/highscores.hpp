#include "common.h"

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <vector>

extern Highscores highscores;
extern App app;

void initHighscoreTable(void);
void initHighscores(void);
void addHighscore(int score);
static void logic(void);
static void draw(void);
static void drawHighscores(void);
static int highscoreComparator(const void *a, const void *b);
static void doNameInput(void);
static void drawNameInput(void);

extern void doBackground(void);
extern void doStarfield(void);
extern void drawBackground(void);
extern void drawStarfield(void);
extern void drawText(int x, int y, int r, int g, int b, int align, char *format, ...);
extern void initStage(void);
extern void initTitle(void);
extern void addScoreToDB(int score, char *name);
extern void readFromDB(std::map<int, std::string, std::greater<int>> &scoreMap);

static Highscore *newHighscore;
static int cursorBlink;
static int timeout;
