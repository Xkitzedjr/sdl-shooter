#include "stage.h"

//initialize stage, called at start
void initStage(void) {
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    bulletTexture = loadTexture("gfx/playerBullet.png");
    enemyTexture = loadTexture("gfx/enemy.png");
    alienBulletTexture = loadTexture("gfx/alienBullet.png");
    playerTexture = loadTexture("gfx/player.png");
    background = loadTexture("gfx/background.png");
    explosionTexture = loadTexture("gfx/explosion.png");
    pointsTexture = loadTexture("gfx/points.png");

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    resetStage();

    stage.score = 0;

    initPlayer();

    enemySpawnTimer = 0;

    stageResetTime = FPS * 3;
}

//game loop for stage
static void logic(void) {
    doBackground();

    doStarfield();

    doPlayer();

    doEnemies();

    doFighters();

    doBullets();

    spawnEnemies();

    doPointsPod();

    clipPlayer();

    if (player == NULL && --stageResetTime <= 0) {
        addHighscore(stage.score);

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
    bullet->texture = bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);

    player->reload = 8;

    bullet->side = SIDE_PLAYER;
}

//bullet logic, kenematics, destroy, free memory, remove from list
static void doBullets(void) {
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

    drawPointsPod();

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

//game logic for all ships, enemy and player. Kenematics, destruction, free memory
//removes from linked list
static void doFighters(void) {
    Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        e->x += e->dx;
        e->y += e->dy;

        // if the entity is not the player and is out of bounds then dead
        if (e != player && e->x < -e->w) e->health = 0;

        // if dead, then die
        if (e->health == 0) {
            if (e == player) {
                player = NULL;
                printf("\nplayer has died\n");
            }

            if (e == stage.fighterTail) stage.fighterTail = prev;

            prev->next = e->next;
            free(e);
            e = prev;
        }

        prev = e;
    }
}

//spawns the enemy ships at the end of the screen
static void spawnEnemies(void) {
    Entity *enemy;

    if (--enemySpawnTimer <= 0) {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % SCREEN_HEIGHT;
        enemy->texture = enemyTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));
        enemy->dy = -100 + (rand() % 200);
        enemy->dy /= 100;

        enemySpawnTimer = 30 + (rand() % 60);

        enemy->side = SIDE_ALIEN;
        enemy->health = 1;
        enemy->reload = FPS * (1 + (rand() % 3));
    }
}

static void drawFighters(void) {
    Entity *e;

    for (e = stage.fighterHead.next ; e != NULL; e = e->next)
        blit(e->texture, e->x, e->y);
}

//determines if a bullet hits an enemy, takes bullet and checks every enemy
//setting both health to 0 if hit
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
                playSound(SND_ALIEN_DIE, CH_ANY);

                addPointsPod(e->x + e->w / 2, e->y + e->h / 2);
            }

            stage.score++;

            highScore = MAX(stage.score, highScore);

            return 1;
        }
    }

    return 0;
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

    while (stage.pointsHead.next) {
        e = stage.pointsHead.next;
        stage.pointsHead.next = e->next;
        free(e);
    }

    memset(&stage, 0, sizeof(Stage));

    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;
    stage.pointsTail = &stage.pointsHead;
}

//basic enemy ai
static void doEnemies(void) {
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {

        if (e != player && player != NULL && --e->reload <= 0) {

            e->y = MIN(MAX(e->y, 0), SCREEN_HEIGHT - e->h);

            if (player != NULL && --e->reload <= 0) {
                fireAlienBullet(e);

                playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
            }
        }
    }
}

//takes an enemy ship and makes it fire at the player
static void fireAlienBullet(Entity *e) {
    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = e->x;
    bullet->y = e->y;
    bullet->health = 1;
    bullet->texture = alienBulletTexture;
    bullet->side = e->side;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->x += (e->w / 2) - (bullet->w / 2);
    bullet->y += (e->h / 2) - (bullet->h / 2);

    calcSlope(player->x + (player->w / 2), player->y + (player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

    bullet->dx *= ALIEN_BULLET_SPEED;
    bullet->dy *= ALIEN_BULLET_SPEED;

    bullet->side = SIDE_ALIEN;

    e->reload = (rand() % FPS * 2);
}

//prevent the player ship from goint outside of play area
static void clipPlayer(void) {
    if (player != NULL) {
        if (player->x < 0) player->x = 0;
        if (player->y < 0) player->y = 0;
        if (player->x > SCREEN_WIDTH / 2) player->x = SCREEN_WIDTH / 2;
        if (player->y > SCREEN_HEIGHT - player->h) player->y = SCREEN_HEIGHT - player->h;
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
    drawText(10, 10, 255, 255, 255, TEXT_LEFT, "SCORE: %03d", stage.score);

    if (stage.score > 0 && stage.score == highScore)
        drawText(960, 10, 255, 0, 255, TEXT_RIGHT, "HIGH SCORE: %03d", highScore);
    else
        drawText(960, 10, 255, 255, 255, TEXT_RIGHT, "HIGH SCORE: %03d", highScore);
}

static void doPointsPod(void) {
    Entity *e, *prev;

    prev = &stage.pointsHead;

    for (e = stage.pointsHead.next; e != NULL; e = e->next) {
        //arena collision
        if (e->x < 0) {
            e->x = 0;
            e->dx = -e->dx;
        }

        if (e->x + e->w > SCREEN_WIDTH) {
            e->x = SCREEN_WIDTH - e->w;
            e->dx = -e->dx;
        }

        if (e->y < 0) {
            e->y = 0;
            e->dy = -e->dy;
        }

        if (e->y + e->h > SCREEN_HEIGHT) {
            e->y = SCREEN_HEIGHT - e->h;
            e->dy = -e->dy;
        }

        e->x += e->dx;
        e->y += e->dy;

        //player collision
        if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h)) {
                e->health = 0;

                stage.score += 10;

                highScore = MAX(stage.score, highScore);

                playSound(SND_POINTS, CH_POINTS);
            }

        //decrement health each frame and check for death
        if (--e->health <= 0) {

            if ( e == stage.pointsTail ) stage.pointsTail = prev;

            prev->next = e->next;
            free(e);
            e = prev;
        }

        prev = e;
    }
}

static void addPointsPod(int x, int y) {
    Entity *e;

    e = malloc(sizeof(Entity));
    memset(e, 0, sizeof(Entity));
    stage.pointsTail->next = e;
    stage.pointsTail =e;

    e->x = x;
    e->y = y;
    e->dx = -(rand() % 5);
    e->dy = (rand() % 5) + (rand() % 5);
    e->health = FPS * 10;                // lasting 10 frames
    e->texture = pointsTexture;

    SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);

    e->x -= e->w /2;
    e->y -= e->h /2;
}

static void drawPointsPod(void) {
    Entity *e;

    for (e = stage.pointsHead.next; e != NULL; e = e->next) {
        if (e->health > (FPS * 2) || e->health % 12 < 6)
            blit(e->texture, e->x, e->y);
    }
}
