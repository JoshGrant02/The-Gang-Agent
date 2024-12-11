//dealer.c
//Josh Grant
//12/08/2023

/*
 * This file has the logic for the dealer thread
 */

#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "tableManager.h"
#include "dealer.h"
#include "deck.h"
#include "stdio.h"
#include "handCalculator.h"

static game_state_t* gameState;

//Sets a file scoped pointer to the game state so it is accessable
void initializeDealer(game_state_t* _gameState)
{
    gameState = _gameState;
    initializeDeck();
}

//The entrypoint for the dealer
void* dealerEntry(void* dealerParam)
{
    //Loop until broken out of 
    while (1)
    {
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: Shuffling..\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));
        shuffle();
        shuffle();
        shuffle();
        usleep(1000000);
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: Dealing player cards\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));
        //Deal two cards
        for (int i = 0; i <= 1; i++)
        {
            //Deal to each player, one card at a time
            for (int j = 0; j < 10; j++)
            {
                //Make sure the player is at the table before dealing to them
                if (gameState->players[j].atTable)
                {
                    int card = dealCard();
                    pthread_mutex_lock(&(gameState->consoleMutex));
                    pthread_mutex_unlock(&(gameState->consoleMutex));
                    gameState->players[j].cards[i] = card;
                    int signal = i == 0 ? SIGUSR1 : SIGUSR2;
                    union sigval player;
                    player.sival_int = j;
                    //Send a signal to the corresponding butler to alert that a card is dealt
                    int rc = pthread_sigqueue(gameState->players[j].playerThread, signal, player);
                    if (rc != 0)
                    {
                        perror("sending");
                    }
                    usleep(500000);
                }
            }
        }

        //Deal 5 cards to the table
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: Dealing table cards\n");
        burnCard();
        for (int i = 0; i < 5; i++)
        {
            int card = dealCard();
            gameState->table.tableCards[i] = card;
            char valueBuffer[3];
            getCardValue(card, valueBuffer);
            printf("%s ", valueBuffer);
        }
        printf("\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));

        int winningPlayer = -1;
        int winningPlayerRank = 0;
        int winningHand[HANDSIZE];
        //Check each player's hand rank
        for (int i = 0; i < 10; i++)
        {
            //Make sure player is in game
            if (gameState->players[i].atTable)
            {
                //Create a 7 card hand of the 5 on the table and two for the player
                int hand[HANDSIZE];
                for (int cardNum = 0; cardNum < 5; cardNum++)
                {
                    hand[cardNum] = gameState->table.tableCards[cardNum];
                }
                hand[5] = gameState->players[i].cards[0];
                hand[6] = gameState->players[i].cards[1];

                //Get the integer rank of the hand
                int playerRank = calculateRank(hand);
                
                //Compare it to highest rank, and replace if higher
                if (playerRank > winningPlayerRank)
                {
                    winningPlayer = i;
                    winningPlayerRank = playerRank;
                    memcpy(winningHand, hand, sizeof(int)*HANDSIZE);
                }
                #ifdef DEBUG
                printf("player %d: %d\n\n", i, playerRank);
                #endif
            }
        }

        //Check to figure out what hand the winning player had
        char handName[12];
        switch (winningPlayerRank & HANDINDICATOR)
        {
            case ROYALFLUSH:
                strcpy(handName, "Royal Flush");
                break;
            case QUADS:
                strcpy(handName, "Quads");
                break;
            case FULLHOUSE:
                strcpy(handName, "Full House");
                break;
            case FLUSH:
                strcpy(handName, "Flush");
                break;
            case STRAIGHT:
                strcpy(handName, "Straight");
                break;
            case TRIPS:
                strcpy(handName, "Trips");
                break;
            case TWOPAIR:
                strcpy(handName, "Two Pair");
                break;
            default:
                if ((winningPlayerRank & PAIRINDICATOR) == PAIR)
                {
                    strcpy(handName, "Pair");
                }
                else
                {
                    strcpy(handName, "High Card");
                }
        }

        //Print out winning hand rank and cards
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: Player %d won with a %s:\n", winningPlayer, handName);
        for (int i = 0; i < HANDSIZE; i++)
        {
            if (winningHand[i] != 0)
            {
                char valueBuffer[3];
                getCardValue(winningHand[i], valueBuffer);
                printf("%s ", valueBuffer);
            }
        }
        printf("\n\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));

        //Make sure the game is still going
        if (gameState->closingTime) break;

        //Wait 10 sec before looping back to redeal
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: The next hand will start in 10 seconds..\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));
        usleep(7000000);
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: The next hand will start in 3 seconds..\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));
        usleep(1000000);
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: The next hand will start in 2 seconds..\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));
        usleep(1000000);
        pthread_mutex_lock(&(gameState->consoleMutex));
        printf("DEALER: The next hand will start in 1 seconds..\n");
        pthread_mutex_unlock(&(gameState->consoleMutex));
        usleep(1000000);
    }
    gameState->table.isActive = 0;
    pthread_mutex_lock(&(gameState->consoleMutex));
    printf("DEALER: The table is closing. Goodnight all!\n");
    pthread_mutex_unlock(&(gameState->consoleMutex));
    return (void*) 0;
}