#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include "playerServant.h"
#include "tableManager.h"

static game_state_t* gameState;

void initializePlayers(game_state_t* _gameState)
{
    gameState = _gameState;
}

void* playerThread(void* player_param)
{
    printf("A player has joined the lobby\n");
    int player = (int) player_param;
    char message[100];
    char buffer[100];

    message[0] = 0;
    buffer[0] = 0;

    while (strcmp(message, "quit\n") != 0)
    {
        ssize_t messageSize = recv(player, buffer, 100, 0);
        strncpy(message, buffer, messageSize);
        message[messageSize] = 0;//Adding null terminator because strncpy is dumb
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("recieving message from %d: %s\n", player, message);
        pthread_mutex_unlock(&(gameState->consoleMutex));
    }
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("Player is quitting\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    close(player);
}