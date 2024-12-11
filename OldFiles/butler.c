//butler.c
//Josh Grant
//12/08/2023

/*
 * This file manages the connection between a player and the table
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "butler.h"
#include "tableManager.h"

static game_state_t* gameState;

//Sets a file scoped pointer to the game state so it is accessable
void initializePlayers(game_state_t* _gameState)
{
    gameState = _gameState;
}

//Signal handler for SIGUSR1 and SIGUSR2 for butler thread
void distributeCard(int signal, siginfo_t* info, void* ucontext)
{
    int player = info->si_value.sival_int;
    player_state_t* playerState = &(gameState->players[player]);
    int card;
    char buffer[sizeof(int)];
    //Character is char 0
    if (signal == SIGUSR1)
    {
        card = playerState->cards[0];
    }
    //Character is char 1
    else if (signal == SIGUSR2)
    {
        card = playerState->cards[1];
    }
    //Convert character to buffer of bytes
    for (size_t i = 0; i < sizeof(int); ++i)
    {
        buffer[i] = (card >> (8 * i)) & 0xFF;
    }
    int numSent = send(playerState->playerSocket, buffer, sizeof(int), 0);
    //Make sure number is sent
    if (numSent < 0)
    {
        perror("Failed to send card");
    }
}

//Entrypoint for bulter thread
void* playerThread(void* playerNum)
{
    int player = (int) playerNum;
    player_state_t* playerState = &(gameState->players[player]);
    playerState->atTable = 1;

    /*
    char message[100];
    char buffer[100];

    while (strcmp(message, "quit\n") != 0)
    {
        ssize_t messageSize = recvfrom(playerState->playerSocket, buffer, 100, 0, &(gameState->playerAddress), sizeof(struct sockaddr_un));
        buffer[messageSize] = 0;
        //strncpy(message, buffer, messageSize);
        //message[messageSize] = 0;//Adding null terminator because strncpy is dumb
        //pthread_mutex_lock(&(gameState->consoleMutex));
        //printf("Recieving message from %d: %s\n", player, message);
        //pthread_mutex_unlock(&(gameState->consoleMutex));
    }
    */

    //Wait until its closing time and the table has finished
    while (!(gameState->closingTime) || gameState->table.isActive);

    //Tell the player that the game is over
    int closingTime = -1;
    char buffer[sizeof(int)];
    for (size_t i = 0; i < sizeof(int); ++i)
    {
        buffer[i] = (closingTime >> (8 * i)) & 0xFF;
    }
    send(playerState->playerSocket, buffer, sizeof(int), 0);

    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("Player %d is leaving\n", player);
    pthread_mutex_unlock(&(gameState->consoleMutex));
    close(player);

    return (void*) 0;
}