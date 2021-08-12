#include "title.h"

void initTitle(void) {
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    sdl2Texture = loadTexture("gfx/sdl2.png");
    shooterTexture = loadTexture("gfx/shooter.png");

    timeout = FPS * 8;
}

static void logic(void) {
    doBackground();

    doStarfield();

    if (reveal < SCREEN_HEIGHT)
        reveal++;
    if (--timeout <= 0)
        initHighscore();
    if (app.keyboard[SDL_SCANCODE_SPACE])
        initStage();
    if (app.keyboard[SDL_SCANCODE_M])
        initMulti();
    if (app.keyboard[SDL_SCANCODE_P]) {
        initConnection();
        initMulti_Net();
    }
}

static void draw(void) {
    drawBackground();

    drawStarfield();

    drawLogo();

    if (timeout % 40 < 20)
        drawText(SCREEN_WIDTH / 2, 400, 255, 255, 255, TEXT_CENTER, "PRESS FIRE TO PLAY!");
        drawText(SCREEN_WIDTH / 2, 500, 255, 255, 255, TEXT_CENTER, "PRESS M FOR MULTIPLAY");
        drawText(SCREEN_WIDTH / 2, 600, 255, 255, 255, TEXT_CENTER, "PRESS P FOR NETPLAY");
}

static void drawLogo(void) {
    SDL_Rect r;

    r.x = 0;
    r.y = 0;

    SDL_QueryTexture(sdl2Texture, NULL , NULL, &r.w, &r.h);

    r.h = MIN(reveal, r.h);

    blitRect(sdl2Texture, &r, (SCREEN_WIDTH / 2) - (r.w / 2), 100);

    SDL_QueryTexture(shooterTexture, NULL, NULL, &r.w, &r.h);

    r.h = MIN(reveal, r.h);

    blitRect(shooterTexture, &r, (SCREEN_WIDTH / 2) - (r.w / 2), 250);
}
