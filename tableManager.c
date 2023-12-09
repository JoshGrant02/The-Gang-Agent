//tableManager.c
//Josh Grant
//12/08/2023

/*
 * This file contains the main entrypoint for The Poker Table and manages console commands
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include "tableManager.h"
#include "dealer.h"
#include "bouncer.h"
#include "butler.h"

#define BOUNCER_NAME "/tmp/pokerbouncer"

static char thePokerTable[] = {36, 36, 36, 36, 36, 36, 36, 36, 92, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 92, 95, 95, 36, 36, 32, 32, 95, 95, 124, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 95, 95, 36, 36, 32, 32, 95, 95, 124, 32, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 92, 32, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 36, 36, 36, 36, 36, 32, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 124, 32, 36, 36, 32, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 124, 32, 92, 95, 95, 95, 95, 36, 36, 92, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 32, 32, 95, 95, 95, 95, 47, 32, 36, 36, 32, 47, 32, 32, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 32, 32, 47, 32, 36, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 32, 124, 32, 32, 92, 95, 95, 124, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 36, 36, 32, 124, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 32, 32, 95, 95, 95, 95, 124, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 32, 95, 36, 36, 60, 32, 32, 36, 36, 32, 32, 32, 95, 95, 95, 95, 124, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 32, 124, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 124, 36, 36, 32, 32, 32, 95, 95, 95, 95, 124, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 92, 36, 36, 36, 36, 36, 36, 36, 92, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 92, 36, 36, 36, 36, 36, 36, 32, 32, 124, 36, 36, 32, 124, 32, 92, 36, 36, 92, 32, 92, 36, 36, 36, 36, 36, 36, 36, 92, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 92, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 36, 32, 32, 124, 36, 36, 32, 124, 92, 36, 36, 36, 36, 36, 36, 36, 92, 32, 10, 32, 32, 32, 92, 95, 95, 124, 32, 32, 32, 92, 95, 95, 124, 32, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124, 92, 95, 95, 124, 32, 32, 32, 32, 32, 32, 32, 92, 95, 95, 95, 95, 95, 95, 47, 32, 92, 95, 95, 124, 32, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124, 92, 95, 95, 124, 32, 32, 32, 32, 32, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124, 92, 95, 95, 95, 95, 95, 95, 95, 47, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124};

static game_state_t* gameState;

//Entrypoint
int main(int argc, char** argv)
{
    //Print logo
    printf("Welcome to\n%s\n", thePokerTable);

    //Create gmae state
    gameState = malloc(sizeof(game_state_t));
    pthread_mutex_init(&(gameState->consoleMutex), NULL);

    #ifdef DEBUG
    printf("Game State: %d\n", gameState);
    printf("Mutex: %d\n", gameState->consoleMutex);
    #endif

    //Give game state to other files
    initializeDealer(gameState);
    initializeBouncer(gameState);
    initializePlayers(gameState);

    pthread_t bouncer = 0;
    pthread_t dealer = 0;

    //Create teh bouncer
    pthread_create(&bouncer, NULL, bouncerEntry, (void*)0);

    char buffer[100];
    buffer[0] = 0;

    //Loop, accepting commands
    while (1)
    {
        fgets(buffer, 100, stdin);
        //Start command starts the table
        if (strcmp(buffer, "start\n") == 0)
        {
            pthread_mutex_lock(&(gameState->consoleMutex));
            //Make sure we haven't started
            if (gameState->table.isActive)
            {
                printf("MANAGER: The game is already running\n");
            }
            else
            {
                //Make sure we have enough players
                if (gameState->playerCount < 2)
                {
                    printf("MANAGER: There are not enough players in the lobby. Try again later\n");
                }
                //If we have enough, create a dealer
                else
                {
                    printf("MANAGER: The game is starting\n");
                    gameState->table.isActive = 1;
                    pthread_create(&dealer, NULL, dealerEntry, (void*)0);
                }
            }
            pthread_mutex_unlock(&(gameState->consoleMutex));
        }
        //Quit command quits the table
        else if (strcmp(buffer, "quit\n") == 0)
        {
            pthread_mutex_lock(&(gameState->consoleMutex));
            printf("MANAGER: The table will close after this hand\n");
            pthread_mutex_unlock(&(gameState->consoleMutex));
            gameState->closingTime = 1;
            break;
        }
    }

    //Rejoin the threads
    pthread_join(dealer, NULL);
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("I collected the dealer\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));

    pthread_join(bouncer, NULL);
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("I collected the bouncer\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    free(gameState);
}