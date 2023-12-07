#ifndef H_PLAYERSERVANT
#define H_PLAYERSERVANT

#include "tableManager.h"

void initializePlayers(game_state_t* _gameState);
void* playerThread(void* player_param);

#endif