//bouncer.c
//Josh Grant
//12/08/2023

/*
 * This file watches for connecting players and creates butlers for each connection
 */

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include "butler.h"
#include "tableManager.h"

#define BOUNCER_NAME "/tmp/pokerbouncer"
#define CLIENT_NAME "/tmp/player"

static game_state_t* gameState;

//Sets a file scoped pointer to the game state so it is accessable
void initializeBouncer(game_state_t* _gameState)
{
    gameState = _gameState;
}

//Entry function for the bouncer
void* bouncerEntry(void* param)
{
    #ifdef DEBUG
    printf("Game State: %d\n", gameState);
    printf("Mutex: %d\n", gameState->consoleMutex);
    #endif

    //Create socket
    int listener = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    unlink(BOUNCER_NAME);

    //Store addresses in gameState
    struct sockaddr_un listener_address;
    listener_address.sun_family = AF_UNIX;
    strcpy(listener_address.sun_path, BOUNCER_NAME);
    memcpy(&(gameState->bouncerAddress), &listener_address, sizeof(struct sockaddr_un));

    struct sockaddr_un player_address;
    player_address.sun_family = AF_UNIX;
    strcpy(player_address.sun_path, CLIENT_NAME);
    memcpy(&(gameState->playerAddress), &player_address, sizeof(struct sockaddr_un));

    //Bind socket
    bind(listener, (struct sockaddr*) &listener_address, sizeof(struct sockaddr_un));
    listen(listener, 10);

    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("BOUNCER: I'm waiting for players\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));

    //Register signal handler for card distribution
    struct sigaction sa;
    sa.sa_sigaction = distributeCard;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);//Register handler for SIGUSR1
    int rc = sigaction(SIGUSR2, &sa, NULL);//Register handler for SIGUSR2
    if (rc < 0)
    {
        perror("registering");
    }

    //Listen for players
    gameState->playerCount = 0;
    socklen_t listener_size = sizeof(struct sockaddr_un);
    while(gameState->playerCount < 10)
    {
        //Nonblocking accept returns -1 if no new connection
        int playerSocket = accept(listener, (struct sockaddr*) &listener_address, &listener_size);
        //If no new connection, watch for the end of the game
        if (playerSocket == -1)
        {
            if (gameState->closingTime)
            {
                break;
            }
        }
        //If new connection, make a thread for it
        else
        {
            gameState->players[gameState->playerCount].playerSocket = playerSocket;
            pthread_mutex_lock(&(gameState->consoleMutex));
            printf("BOUNCER: A player has joined the lobby\n");
            pthread_mutex_unlock(&(gameState->consoleMutex));
            pthread_create(&(gameState->players[gameState->playerCount].playerThread), NULL, playerThread, (void*) gameState->playerCount);//TODO: Change id
            gameState->playerCount++;
        }
    }

    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("BOUNCER: I will start escorting people out\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    
    //Join back all player threads
    for (int i = 0; i < 10; i++)
    {
        //Only join the threads that exist
        if (gameState->players[i].atTable)
        {
            pthread_mutex_lock(&(gameState->consoleMutex));
            printf("BOUNCER: Waiting for player %d\n", i);
            pthread_mutex_unlock(&(gameState->consoleMutex));
            pthread_join(gameState->players[i].playerThread, NULL);
            pthread_mutex_lock(&(gameState->consoleMutex));
            printf("BOUNCER: Player %d has left\n", i);
            pthread_mutex_unlock(&(gameState->consoleMutex));
        }
    }
    
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("BOUNCER: I've kicked everyone out\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    return (void*) 0;
}