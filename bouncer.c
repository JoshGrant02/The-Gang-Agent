#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include "playerServant.h"
#include "tableManager.h"

#define BOUNCER_NAME "/tmp/pokerbouncer"

static game_state_t* gameState;

void initializeBouncer(game_state_t* _gameState)
{
    gameState = _gameState;
}

void* bouncerEntry(void* playerParam)
{
    int listener = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(BOUNCER_NAME);
    
    struct sockaddr_un listener_address;
    listener_address.sun_family = AF_UNIX;
    strcpy(listener_address.sun_path, BOUNCER_NAME);

    bind(listener, (struct sockaddr*) &listener_address, sizeof(struct sockaddr_un));
    listen(listener, 10);

    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("BOUNCER: I'm waiting for players\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));

    int player;
    while(gameState->playerCount < 5)
    {
        socklen_t listener_size = sizeof(struct sockaddr_un);
        player = accept(listener, (struct sockaddr*) &listener_address, &listener_size);
        pthread_create(&(gameState->players[gameState->playerCount].playerThread), NULL, playerThread, (void*) player);
        gameState->playerCount++;
    }

    pthread_join(gameState->players[gameState->playerCount].playerThread, NULL);
    printf("I'm done waiting for people\n");
}