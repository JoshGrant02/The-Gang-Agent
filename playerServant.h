#ifndef H_PLAYERSERVANT
#define H_PLAYERSERVANT

#include <signal.h>
#include "tableManager.h"

void initializePlayers(game_state_t* _gameState);
void distributeCard(int signal, siginfo_t* info, void* ucontext);
void* playerThread(void* playerNum);

#endif