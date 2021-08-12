#include "multi_net.hpp"
// TODO
// [X] all p2 logic needs to be gotten from local
// [X] all p1 logic needs to be sent to remote server
// [X] create a static var OP_CODE to hold current p1 status
// [] fix bug where game crashes on death

//initialize stage, called at start
void initMulti_Net(void) {
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(M_Stage));
    stage.player1 = NULL;
    stage.player2 = NULL;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    p1bulletTexture = loadTexture((char *)"gfx/playerBullet.png");
    player2Texture = loadTexture((char *)"gfx/enemy.png");
    p2BulletTexture = loadTexture((char *)"gfx/alienBullet.png");
    playerTexture = loadTexture((char *)"gfx/player.png");
    background = loadTexture((char *)"gfx/background.png");
    explosionTexture = loadTexture((char *)"gfx/explosion.png");

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

    doBullets();

    clipPlayer();

    if ( (stage.player1 == NULL || stage.player2 == NULL) && --stageResetTime <= 0 ) {
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
    if (stage.player1 != NULL) {
        if (stage.player1->health == 0) {
            std::cout << "\nPlayer 1 has died\nPlayer 2 has scored a point" << std::endl;
            p2Score++;

            free(stage.player1);
            stage.player1 = NULL;
            updateToBeSent(0, 0, OP_CODE::DEATH);
        }

        else {
            stage.player1->dx = stage.player1->dy = 0;

            if (stage.player1->reload > 0) stage.player1->reload--;

            if (app.keyboard[SDL_SCANCODE_UP]) stage.player1->dy = -PLAYER_SPEED;
            if (app.keyboard[SDL_SCANCODE_DOWN]) stage.player1->dy = PLAYER_SPEED;
            if (app.keyboard[SDL_SCANCODE_RIGHT]) stage.player1->dx = PLAYER_SPEED;
            if (app.keyboard[SDL_SCANCODE_LEFT]) stage.player1->dx = -PLAYER_SPEED;

            if (app.keyboard[SDL_SCANCODE_SPACE] && stage.player1->reload <= 0) {
                fireBullet();
                playSound(SND_PLAYER_FIRE, CH_PLAYER);
                currentStatus = OP_CODE::UPDATE_POS_AND_FIRE;
            }
            else {
                currentStatus = OP_CODE::UPDATE_POS;
            }

            stage.player1->x += stage.player1->dx;
            stage.player1->y += stage.player1->dy;

            updateToBeSent(stage.player1->x, stage.player1->y, currentStatus);
        }
    }
}

static void doPlayer2(void) {
    if (stage.player2 != NULL) {
        stage.player2->dx = stage.player2->dy = 0;

        Message *p2status = new Message();
        getLastRecieved(p2status);

        if (p2status->message == OP_CODE::DEATH) {
            std::cout << "\nPlayer 2 has died\nPlayer 1 has scored a point" << std::endl;
            p1Score++;

            addExplosions(stage.player2->x, stage.player2->y, 32);

            addDebris(stage.player2);

            playSound(SND_PLAYER_DIE, CH_ANY);

            free(stage.player2);
            stage.player2 = NULL;
        }

        else {
            stage.player2->x = p2status->x + SCREEN_WIDTH / 2;
            stage.player2->y = p2status->y;

            if (p2status->message == OP_CODE::UPDATE_POS_AND_FIRE) {
                p2fireBullet();
                playSound(SND_PLAYER_FIRE, CH_PLAYER); //TODO sound channel
            }
        }

        delete p2status;
    }
}

//when the PLAYER ONLY fires a bullet
static void fireBullet() {
    Entity *bullet;

    bullet = static_cast<Entity *>(malloc(sizeof(Entity)));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = stage.player1->x;
    bullet->y = stage.player1->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = p1bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (stage.player1->h / 2) - (bullet->h / 2);

    stage.player1->reload = 8;

    bullet->side = SIDE_PLAYER;
}

static void p2fireBullet() {
    Entity *bullet;

    bullet = static_cast<Entity *>(malloc(sizeof(Entity)));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = stage.player2->x;
    bullet->y = stage.player2->y;
    bullet->dx = -PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = p2BulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (stage.player2->h / 2) - (bullet->h / 2);

    stage.player2->reload = 8;

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
    Entity *player = static_cast<Entity *>(malloc(sizeof(Entity)));
    memset(player, 0, sizeof(Entity));
    stage.player1 = player;

    player->health = 1;
    player->x = 100;
    player->y = 100;
    player->texture = playerTexture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);

    player->side = SIDE_PLAYER;
}

static void initPlayer2() {
    Entity *player2 = static_cast<Entity *>(malloc(sizeof(Entity)));
    memset(player2, 0, sizeof(Entity));
    stage.player2 = player2;

    player2->health = 1;
    player2->x = SCREEN_WIDTH - 100;
    player2->y = 100;
    player2->texture = player2Texture;
    SDL_QueryTexture(player2->texture, NULL, NULL, &player2->w, &player2->h);

    player2->side = SIDE_PLAYER2;
}

static void drawFighters(void) {
    blit(stage.player1->texture, stage.player1->x, stage.player1->y);
    blit(stage.player2->texture, stage.player2->x, stage.player2->y);
}

//reset stage on player death
static void resetStage(void) {
    Entity *e;
    Explosion *ex;
    Debris *d;

    if (stage.player1 != NULL) {
        free(stage.player1);
        stage.player1 = NULL;
    }

    if (stage.player2 != NULL) {
        free(stage.player2);
        stage.player2 = NULL;
    }

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

    memset(&stage, 0, sizeof(M_Stage));

    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    stageResetTime = FPS * 3;
}

//prevent the player ship from goint outside of play area
static void clipPlayer(void) {
    if (stage.player1 != NULL) {
        if (stage.player1->x < 0) stage.player1->x = 0;
        if (stage.player1->y < 0) stage.player1->y = 0;
        if (stage.player1->x > SCREEN_WIDTH / 2) stage.player1->x = SCREEN_WIDTH / 2;
        if (stage.player1->y > SCREEN_HEIGHT - stage.player1->h) stage.player1->y = SCREEN_HEIGHT - stage.player1->h;
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
    std::array<Entity *, 2> eArr = {stage.player1, stage.player2};

    // lazy crap
    for (auto e : eArr) {
        if (e == stage.player1 && e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
            b->health = 0;
            e->health = 0;

            addExplosions(e->x, e->y, 32);

            addDebris(e);

            if (e == stage.player1)
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
        e = static_cast<Explosion *>(malloc(sizeof(Explosion)));
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
            d = static_cast<Debris *>(malloc(sizeof(Debris)));
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

    drawText(10, 10, 255, 255, 255, TEXT_LEFT, (char *)"PLAYER 1 SCORE: %03d", p1Score);

    drawText(960, 10, 255, 0, 255, TEXT_RIGHT, (char *)"PLAYER 2 SCORE: %03d", p2Score);

}

/*
static void removeFighter(Entity *e) {
    Entity *p = &stage.fighterHead;

    if (e == p)
        stage.fighterHead = *stage.fighterHead.next;
    else {
        while (p->next != e) p = p->next;
        p->next = e->next;
    }

    free(e);
    e = NULL;
}
*/
