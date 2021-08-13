#include "multi_net.hpp"
// TODO
// [X] all p2 logic needs to be gotten from local
// [X] all p1 logic needs to be sent to remote server
// [X] create a static var OP_CODE to hold current p1 status
// [X] fix bug where game crashes on death

//initialize stage, called at start
void initMulti_Net(void) {
    app.delegate.logic = logic;
    app.delegate.draw = draw;

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

    p1score = p2score = 0;

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

    if ( (!stage.player1 || !stage.player2) && --stageResetTime <= 0 ) {
        resetStage();
        initPlayer();
        initPlayer2();
    }

    if ( (p1score == MULTI_SCORE_LIMIT || p2score == MULTI_SCORE_LIMIT) && --stageResetTime <= 0) {
        initHighscore();
    }

    doExplosion();

    doDebris();
}

//game logic for player, checks input and responds
static void doPlayer(void) {
    if (stage.player1) {
        if (stage.player1->health == 0) {
            std::cout << "\nPlayer 1 has died\nPlayer 2 has scored a point" << std::endl;
            p2score++;

            stage.player1.reset();
            updateToBeSent(0, 0, OP_CODE::DEATH);
        }

        else {
            OP_CODE currentStatus;
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
    if (stage.player2) {
        stage.player2->dx = stage.player2->dy = 0;

        std::unique_ptr<Message> p2status{ new Message() };
        getLastRecieved(p2status);

        if (p2status->message == OP_CODE::DEATH) {
            std::cout << "\nPlayer 2 has died\nPlayer 1 has scored a point" << std::endl;
            p1score++;

            addExplosions(stage.player2->x, stage.player2->y, 32);

            addDebris(stage.player2);

            playSound(SND_PLAYER_DIE, CH_ANY);

            stage.player2.reset();
        }

        else {
            stage.player2->x = p2status->x + SCREEN_WIDTH / 2;
            stage.player2->y = p2status->y;

            if (p2status->message == OP_CODE::UPDATE_POS_AND_FIRE) {
                p2fireBullet();
                playSound(SND_PLAYER_FIRE, CH_PLAYER); //TODO sound channel
            }
        }
    }
}

//when the PLAYER ONLY fires a bullet
static void fireBullet() {
    UP<Entity> bullet{ new Entity() };

    bullet->x = stage.player1->x;
    bullet->y = stage.player1->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = p1bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (stage.player1->h / 2) - (bullet->h / 2);

    stage.player1->reload = 8;

    bullet->side = SIDE_PLAYER;

    stage.bullets.push_back(std::move(bullet));
}

static void p2fireBullet() {
    UP<Entity> bullet{ new Entity() };

    bullet->x = stage.player2->x;
    bullet->y = stage.player2->y;
    bullet->dx = -PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = p2BulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (stage.player2->h / 2) - (bullet->h / 2);

    stage.player2->reload = 8;

    bullet->side = SIDE_PLAYER2;

    stage.bullets.push_back(std::move(bullet));
}

//bullet logic, kenematics, destroy, free memory, remove from list
static void doBullets(void)
{
    for (auto b = stage.bullets.begin() ; b != stage.bullets.end(); b++) {
        (*b)->x += (*b)->dx;
        (*b)->y += (*b)->dy;

        if (bulletHitFighter(*b) || (*b)->x > SCREEN_WIDTH || (*b)->y > SCREEN_HEIGHT || (*b)->x < -(*b)->w || (*b)->y < -(*b)->h) {
            b = stage.bullets.erase(b);
        }
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
    for (auto &&b : stage.bullets) {
        blit(b->texture, b->x, b->y);
    }
}

//create player ship, called on start or when player dies
static void initPlayer(void) {
    stage.player1 = UP<Entity> { new Entity() };

    stage.player1->health = 1;
    stage.player1->x = 100;
    stage.player1->y = 100;
    stage.player1->texture = playerTexture;
    SDL_QueryTexture(stage.player1->texture, NULL, NULL, &stage.player1->w, &stage.player1->h);

    stage.player1->side = SIDE_PLAYER;
}

static void initPlayer2(void) {
    stage.player2 = UP<Entity> { new Entity() };

    stage.player2->health = 1;
    stage.player2->x = SCREEN_WIDTH - 100;
    stage.player2->y = 100;
    stage.player2->texture = player2Texture;
    SDL_QueryTexture(stage.player2->texture, NULL, NULL, &stage.player2->w, &stage.player2->h);

    stage.player2->side = SIDE_PLAYER2;
}

static void drawFighters(void) {
    if (stage.player1)
        blit(stage.player1->texture, stage.player1->x, stage.player1->y);
    if (stage.player2)
        blit(stage.player2->texture, stage.player2->x, stage.player2->y);
}

//reset stage on player death
static void resetStage(void) {
    stage.player1.reset();
    stage.player2.reset();

    stage.bullets.clear();
    stage.explosions.clear();
    stage.debris.clear();

    stageResetTime = FPS * 3;
}

//prevent the player ship from goint outside of play area
static void clipPlayer(void) {
    if (stage.player1) {
        if (stage.player1->x < 0) stage.player1->x = 0;
        if (stage.player1->y < 0) stage.player1->y = 0;
        if (stage.player1->x > SCREEN_WIDTH / 2) stage.player1->x = SCREEN_WIDTH / 2;
        if (stage.player1->y > SCREEN_HEIGHT - stage.player1->h) stage.player1->y = SCREEN_HEIGHT - stage.player1->h;
    }
}

static void doExplosion(void) {
    //make explosion spread out
    for (auto e = stage.explosions.begin(); e != stage.explosions.end(); e++) {
        (*e)->x += (*e)->dx;
        (*e)->y += (*e)->dy;

        //if alpha is <=0 it is completely transparent and is freed
        //and removed from list
        if (--(*e)->a <= 0) {
            e = stage.explosions.erase(e);
        }
    }
}

static int bulletHitFighter(UP<Entity> &b) {
    if (!stage.player1) return 0;

    auto &e = stage.player1;
    if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
                b->health = 0;
                e->health = 0;

                addExplosions(e->x, e->y, 32);

                addDebris(e);

                    playSound(SND_PLAYER_DIE, CH_PLAYER);

                    playSound(SND_PLAYER_DIE, CH_ANY);
                return 1;
            }
    return 0;
}

static void doDebris(void) {
    for (auto d = stage.debris.begin(); d != stage.debris.end(); d++) {
        (*d)->x += (*d)->dx;
        (*d)->y += (*d)->dy;

        (*d)->dy *= 0.99;
        (*d)->dx *= 0.99;

        if (--(*d)->life <= 0) {
            d = stage.debris.erase(d);
        }
    }
}

static void addExplosions(int x, int y, int num) {
    for (int i = 0; i < num; i++){
        UP<Explosion> e = UP<Explosion>{new Explosion()};

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

        stage.explosions.push_back(std::move(e));
    }
}

static void addDebris(UP<Entity> &e) {
    int x, y, w, h;

    w = e->w / 2;
    h = e->h / 2;

    for (y = 0; y <= h; y += h) {
        for (x = 0; x <= w; x += w) {
            UP<Debris> d = UP<Debris>{ new Debris() };

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

            stage.debris.push_back(std::move(d));
        }
    }
}

static void drawDebris(void) {
    for (auto &&d : stage.debris) {
        blitRect(d->texture, &d->rect, d->x, d->y);
    }
}

static void drawExplosions(void) {
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);

    for (auto &&e : stage.explosions) {
        SDL_SetTextureColorMod(explosionTexture, e->r, e->g, e->b);
        SDL_SetTextureAlphaMod(explosionTexture, e->a);

        blit(explosionTexture, e->x, e->y);
    }

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

static void drawHud(void) {

    drawText(10, 10, 255, 255, 255, TEXT_LEFT, (char *)"PLAYER 1 SCORE: %03d", p1score);

    drawText(960, 10, 255, 0, 255, TEXT_RIGHT, (char *)"PLAYER 2 SCORE: %03d", p2score);

}

