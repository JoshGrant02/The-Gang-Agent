//bouncer.h
//Josh Grant
//12/08/2023

/*
 * This file contains function prototypes for the bouncer
 */

#ifndef H_BOUNCER
#define H_BOUNCER

#include "tableManager.h"

void initializeBouncer(game_state_t* _gameState);
void* bouncerEntry(void* param);

#endif