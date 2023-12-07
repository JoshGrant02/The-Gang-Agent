#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "playerServant.h"
#include "tableManager.h"

static game_state_t* gameState;

void initializePlayers(game_state_t* _gameState)
{
    gameState = _gameState;
}

void distributeCard(int signal, siginfo_t* info, void* ucontext)
{
    int player = info->si_value.sival_int;
    player_state_t* playerState = &(gameState->players[player]);
    int card;
    char buffer[sizeof(int)];
    if (signal == SIGUSR1)
    {
        card = playerState->cards[0];
    }
    else if (signal == SIGUSR2)
    {
        card = playerState->cards[1];
    }
    for (size_t i = 0; i < sizeof(int); ++i)
    {
        buffer[i] = (card >> (8 * i)) & 0xFF;
    }
    int numSent = send(playerState->playerSocket, buffer, sizeof(int), 0);
    if (numSent < 0)
    {
        perror("Failed to send card");
    }
}

void* playerThread(void* playerNum)
{
    int player = (int) playerNum;
    player_state_t* playerState = &(gameState->players[player]);
    playerState->atTable = 1;
    char message[100];
    char buffer[100];

    while (strcmp(message, "quit\n") != 0)
    {
        ssize_t messageSize = recv(playerState->playerSocket, buffer, 100, 0);
        buffer[messageSize] = 0;
        //strncpy(message, buffer, messageSize);
        //message[messageSize] = 0;//Adding null terminator because strncpy is dumb
        //pthread_mutex_lock(&(gameState->consoleMutex));
        //printf("recieving message from %d: %s\n", player, message);
        //pthread_mutex_unlock(&(gameState->consoleMutex));
    }
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("Player is quitting\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    close(player);
}