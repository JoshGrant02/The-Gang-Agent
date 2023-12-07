#ifndef H_BOUNCER
#define H_BOUNCER

#include "tableManager.h"

void initializeBouncer(game_state_t* _gameState);
void* bouncerEntry(void* playerParam);

#endif