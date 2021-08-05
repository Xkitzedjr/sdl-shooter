#include "common.h"

#include "SDL2/SDL_mixer.h"

static void loadSounds(void);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;
