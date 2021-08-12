typedef struct Entity Entity;
typedef struct Explosion Explosion;
typedef struct Debris Debris;
typedef struct Texture Texture;

struct Explosion {
	float x, y, dx, dy;
	int r, g, b, a;
	Explosion *next;
};

struct Debris {
	float x, y, dx, dy;
	SDL_Rect rect;
	SDL_Texture *texture;
	int life;
	Debris *next;
};

typedef struct {
	// function pointers
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

struct Texture {
	char name[MAX_NAME_LENGTH];
	SDL_Texture *texture;
	Texture *next;
};

typedef struct {
	//SDL Rendering
	SDL_Renderer *renderer;
	SDL_Window *window;
	Texture textureHead, *textureTail;

	// function pointer struct
	Delegate delegate;

	int keyboard[MAX_KEYBOARD_KEYS];
	char inputText[MAX_LINE_LENGTH];
} App;

struct Entity {
	float x;
	float y;
	int w;
	int h;
	float dx;
	float dy;
	int health;
	int reload;
	int side;

	SDL_Texture *texture;
	Entity *next;
};

typedef struct {
	//stores all the game objects as linked lists
	Entity fighterHead, *fighterTail;
	Entity bulletHead, *bulletTail;
	Entity pointsHead, *pointsTail;
	Explosion explosionHead, *explosionTail;
	Debris debrisHead, *debrisTail;

	int score;
} Stage;

typedef struct {
	//stores all the game objects as linked lists
	Entity *player1;
	Entity *player2;
	Entity bulletHead, *bulletTail;
	Entity pointsHead, *pointsTail;
	Explosion explosionHead, *explosionTail;
	Debris debrisHead, *debrisTail;

	int score;
} M_Stage;

typedef struct {
	int x, y, speed;
} Star;

typedef struct {
	char name[MAX_SCORE_NAME_LENGTH];
	int recent;
	int score;
} Highscore;

typedef struct {
	Highscore highscore[NUM_HIGHSCORES];
} Highscores;
