#ifndef H_DEALER
#define H_DEALER

#include "tableManager.h"

void initializeDealer(game_state_t* _gameState);
void* dealerEntry(void* dealerParam);

#endif