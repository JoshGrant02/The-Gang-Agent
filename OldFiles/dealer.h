//dealer.h
//Josh Grant
//12/08/2023

/*
 * This file contains function prototypes for the dealer
 */

#ifndef H_DEALER
#define H_DEALER

#include "tableManager.h"

void initializeDealer(game_state_t* _gameState);
void* dealerEntry(void* dealerParam);

#endif