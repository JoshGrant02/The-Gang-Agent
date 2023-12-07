#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
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
    gameState->table.isActive = 1;
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("DEALER: Shuffling..\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    shuffle();
    usleep(2000000);
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("DEALER: Dealing player cards\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    for (int i = 0; i <= 1; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (gameState->players[j].atTable)
            {
                int card = dealCard();
                pthread_mutex_lock(&(gameState->consoleMutex));
                pthread_mutex_unlock(&(gameState->consoleMutex));
                gameState->players[j].cards[i] = card;
                int signal = i == 0 ? SIGUSR1 : SIGUSR2;
                union sigval player;
                player.sival_int = j;
                int rc = pthread_sigqueue(gameState->players[j].playerThread, signal, player);
                if (rc != 0)
                {
                    perror("sending");
                }
                usleep(500000);
            }
        }
    }
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("DEALER: Dealing table cards\n");
    for (int i = 0; i < 5; i++)
    {
        int card = dealCard();
        gameState->table.tableCards[i] = card;
        printf("%d ", card);
    }
    printf("\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
}