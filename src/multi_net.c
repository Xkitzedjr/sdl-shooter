#include "multi.h"

//TODO Make the alien a copy of the player

//initialize stage, called at start
void initMulti(void) {
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    p1bulletTexture = loadTexture("gfx/playerBullet.png");
    player2Texture = loadTexture("gfx/enemy.png");
    p2BulletTexture = loadTexture("gfx/alienBullet.png");
    playerTexture = loadTexture("gfx/player.png");
    background = loadTexture("gfx/background.png");
    explosionTexture = loadTexture("gfx/explosion.png");

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    resetStage();

    initPlayer();

    initPlayer2();

    p1Score = p2Score = 0;

    stageResetTime = FPS * 3;
}

//game loop for stage
static void logic(void) {
    doBackground();

    doStarfield();

    doPlayer();

    doPlayer2();

    doFighters();

    doBullets();

    clipPlayer();

    if ( (player == NULL || player2 == NULL) && --stageResetTime <= 0 ) {
        resetStage();
        initPlayer();
        initPlayer2();
    }

    if ( (p1Score == MULTI_SCORE_LIMIT || p2Score == MULTI_SCORE_LIMIT) && --stageResetTime <= 0) {
        initHighscore();
    }

    doExplosion();

    doDebris();
}

//game logic for player, checks input and responds
static void doPlayer(void) {
    if (player != NULL) {
        player->dx = player->dy = 0;

        if (player->reload > 0) player->reload--;

        if (app.keyboard[SDL_SCANCODE_UP]) player->dy = -PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_DOWN]) player->dy = PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_RIGHT]) player->dx = PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_LEFT]) player->dx = -PLAYER_SPEED;

        if (app.keyboard[SDL_SCANCODE_SPACE] && player->reload <= 0) {
            fireBullet();
            playSound(SND_PLAYER_FIRE, CH_PLAYER);
        }
    }
}

static void doPlayer2(void) {
    if (player2 != NULL) {
        player2->dx = player2->dy = 0;

        if (player2->reload > 0) player2->reload--;

        if (app.keyboard[SDL_SCANCODE_W]) player2->dy = -PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_S]) player2->dy = PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_D]) player2->dx = PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_A]) player2->dx = -PLAYER_SPEED;

        if (app.keyboard[SDL_SCANCODE_LCTRL] && player2->reload <= 0) {
            p2fireBullet();
            playSound(SND_PLAYER_FIRE, CH_PLAYER); //TODO sound channel
        }
    }
}


//when the PLAYER ONLY fires a bullet
static void fireBullet() {
    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = p1bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);

    player->reload = 8;

    bullet->side = SIDE_PLAYER;
}

static void p2fireBullet() {
    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = player2->x;
    bullet->y = player2->y;
    bullet->dx = -PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = p2BulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player2->h / 2) - (bullet->h / 2);

    player2->reload = 8;

    bullet->side = SIDE_PLAYER2;
}

//bullet logic, kenematics, destroy, free memory, remove from list
static void doBullets(void)
{
    Entity *b, *prev;

    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next; b != NULL; b = b->next) {
        b->x += b->dx;
        b->y += b->dy;

        if (bulletHitFighter(b) || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT || b->x < -b->w || b->y < -b->h) {

            if (b == stage.bulletTail) stage.bulletTail = prev;

            prev->next = b->next;
            free(b);
            b = prev;
        }
        prev = b;
    }
}

static void draw(void) {
    drawBackground();

    drawStarfield();

    drawFighters();

    drawDebris();

    drawExplosions();

    drawBullets();

    drawHud();
}

static void drawBullets(void) {
    Entity *b;

    for (b = stage.bulletHead.next; b != NULL; b = b->next) {
        blit(b->texture, b->x, b->y);
    }
}

//create player ship, called on start or when player dies
static void initPlayer() {
    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity));
    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->health = 1;
    player->x = 100;
    player->y = 100;
    player->texture = playerTexture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);

    player->side = SIDE_PLAYER;
}

static void initPlayer2() {
    player2 = malloc(sizeof(Entity));
    memset(player2, 0, sizeof(Entity));
    stage.fighterTail->next = player2;
    stage.fighterTail = player2;

    player2->health = 1;
    player2->x = SCREEN_WIDTH - 100;
    player2->y = 100;
    player2->texture = player2Texture;
    SDL_QueryTexture(player2->texture, NULL, NULL, &player2->w, &player2->h);

    player2->side = SIDE_PLAYER2;
}

//game logic for all ships, enemy and player. Kenematics, destruction, free memory
//removes from linked list
static void doFighters(void) {
    Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        e->x += e->dx;
        e->y += e->dy;

        // if dead, then die
        if (e->health == 0) {
            if (e == player) {
                player = NULL;
                printf("\nplayer has died\n");
                p2Score++;
            }
            else {
                player2 = NULL;
                printf("\nplayer 2 has died\n");
                p1Score++;
            }

            if (e == stage.fighterTail) stage.fighterTail = prev;

            prev->next = e->next;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

static void drawFighters(void) {
    Entity *e;

    for (e = stage.fighterHead.next ; e != NULL; e = e->next)
        blit(e->texture, e->x, e->y);
}

//reset stage on player death
static void resetStage(void) {
    Entity *e;

    while (stage.fighterHead.next) {
        e = stage.fighterHead.next;
        stage.fighterHead.next = e->next;
        free(e);
    }

    Explosion *ex;
    Debris *d;

    while (stage.bulletHead.next) {
        e = stage.bulletHead.next;
        stage.bulletHead.next = e->next;
        free(e);
    }

    while (stage.explosionHead.next) {
        ex = stage.explosionHead.next;
        stage.explosionHead.next = ex->next;
        free(ex);
    }

    while (stage.debrisHead.next) {
        d = stage.debrisHead.next;
        stage.debrisHead.next = d->next;
        free(d);
    }

    memset(&stage, 0, sizeof(Stage));

    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    stageResetTime = FPS * 3;
}

//prevent the player ship from goint outside of play area
static void clipPlayer(void) {
    if (player != NULL) {
        if (player->x < 0) player->x = 0;
        if (player->y < 0) player->y = 0;
        if (player->x > SCREEN_WIDTH / 2) player->x = SCREEN_WIDTH / 2;
        if (player->y > SCREEN_HEIGHT - player->h) player->y = SCREEN_HEIGHT - player->h;
    }
    if (player2 != NULL) {
        if (player2->x > SCREEN_WIDTH) player2->x = SCREEN_WIDTH;
        if (player2->y < 0) player2->y = 0;
        if (player2->x < SCREEN_WIDTH / 2) player2->x = SCREEN_WIDTH / 2;
        if (player2->y > SCREEN_HEIGHT - player2->h) player2->y = SCREEN_HEIGHT - player2->h;
    }
}

static void doExplosion(void) {
    Explosion *e, *prev;

    prev = &stage.explosionHead;

    //make explosion spread out
    for (e = stage.explosionHead.next; e != NULL; e = e->next) {
        e->x += e->dx;
        e->y += e->dy;

        //if alpha is <=0 it is completely transparent and is freed
        //and removed from list
        if (--e->a <= 0) {

            if (e == stage.explosionTail)
                stage.explosionTail = prev;

            prev->next = e->next;
            free(e);
            e = prev;
        }

        prev = e;
    }
}
static int bulletHitFighter(Entity *b) {
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
            b->health = 0;
            e->health = 0;

            addExplosions(e->x, e->y, 32);

            addDebris(e);

            if (e == player)
                playSound(SND_PLAYER_DIE, CH_PLAYER);

            else {
                playSound(SND_PLAYER_DIE, CH_ANY);
            }
            return 1;
        }
    }
    return 0;
}

static void doDebris(void) {
    Debris *d, *prev;

    prev = &stage.debrisHead;

    for (d = stage.debrisHead.next; d != NULL; d = d->next) {
        d->x += d->dx;
        d->y += d->dy;

        d->dy *= 0.99;
        d->dx *= 0.99;

        if (--d->life <= 0) {

            if (d == stage.debrisTail)
                stage.debrisTail = prev;

            prev->next = d->next;
            free(d);
            d = prev;
        }

        prev = d;
    }
}

static void addExplosions(int x, int y, int num) {
    Explosion *e;

    for (int i = 0; i < num; i++){
        e = malloc(sizeof(Explosion));
        memset(e, 0, sizeof(Explosion));
        stage.explosionTail->next = e;
        stage.explosionTail = e;

        e->x = x + (rand() % 32) - (rand() % 33);
        e->y = y + (rand() % 32) - (rand() % 33);
        e->dx = (rand() % 10) - (rand() % 10);
        e->dy = (rand() % 10) - (rand() % 10);

        e->dx /= 10;
        e->dy /= 10;

        switch (rand() % 4) {
            case 0:
                e->r = 255;
                break;
            case 1:
                e->r = 255;
                e->g = 128;
                break;
            case 2:
                e->r = 255;
                e->g = 255;
                break;
            default:
                e->r = 255;
                e->g = 255;
                e->b = 255;
                break;
        }

        e->a = rand() % FPS * 3;
    }
}

static void addDebris(Entity *e) {
    Debris *d;
    int x, y, w, h;

    w = e->w / 2;
    h = e->h / 2;

    for (y = 0; y <= h; y += h) {
        for (x = 0; x <= w; x += w) {
            d = malloc(sizeof(Debris));
            memset(d, 0, sizeof(Debris));
            stage.debrisTail->next = d;
            stage.debrisTail = d;

            d->x = e->x + e->w / 2;
            d->y = e->y + e->h /2;
            d->dx = (rand() % 15) - (rand() % 5);
            d->dy = (rand() % 15) - (rand() % 5);
            d->life = FPS * 2;
            d->texture = e->texture;

            d->rect.x = x;
            d->rect.y = y;
            d->rect.h = h;
            d->rect.w = w;
        }
    }
}

static void drawDebris(void) {
    Debris *d;

    for (d = stage.debrisHead.next; d != NULL; d = d->next) {
        blitRect(d->texture, &d->rect, d->x, d->y);
    }
}

static void drawExplosions(void) {
    Explosion *e;

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);

    for (e = stage.explosionHead.next; e != NULL; e = e->next) {
        SDL_SetTextureColorMod(explosionTexture, e->r, e->g, e->b);
        SDL_SetTextureAlphaMod(explosionTexture, e->a);

        blit(explosionTexture, e->x, e->y);
    }

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

static void drawHud(void) {

    drawText(10, 10, 255, 255, 255, TEXT_LEFT, "PLAYER 1 SCORE: %03d", p1Score);

    drawText(960, 10, 255, 0, 255, TEXT_RIGHT, "PLAYER 2 SCORE: %03d", p2Score);

}
