//butler.h
//Josh Grant
//12/08/2023

/*
 * This file contains function prototypes the butler
 */

#ifndef H_BUTLER
#define H_BUTLER

#include <signal.h>
#include "tableManager.h"

void initializePlayers(game_state_t* _gameState);
void distributeCard(int signal, siginfo_t* info, void* ucontext);
void* playerThread(void* playerNum);

#endif