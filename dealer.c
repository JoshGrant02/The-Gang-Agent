#include "tableManager.h"
#include "dealer.h"
#include "deck.h"
#include "stdio.h"

static game_state_t* gameState;

void initializeDealer(game_state_t* _gameState)
{
    gameState = _gameState;
    initializeDeck();
}

void* dealerEntry(void* dealerParam)
{
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("DEALER: I'm going to deal\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    shuffle();
    for (int i = 0; i < 5; i++)
    {
        int card = dealCard();
        gameState->table.tableCards[i] = card;
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: Card %d was dealt\n", card);
        pthread_mutex_unlock(&(gameState->consoleMutex));
    }
}