//tableManager.h
//Josh Grant
//12/08/2023

/*
 * This file contains struct typedefs for the game state
 */

#ifndef H_TABLEMANAGER
#define H_TABLEMANAGER

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct {
    int isActive;
    int tableCards[5];
    int currentPlayer;
    double pot;
} table_state_t;

typedef struct {
    pthread_t playerThread;
    int playerSocket;
    int playerId;
    int atTable;
    int inHand;
    double bet;
    int cards[2];
} player_state_t;

typedef struct {
    int closingTime;
    struct sockaddr bouncerAddress;
    struct sockaddr playerAddress;
    pthread_mutex_t consoleMutex;
    int playerCount;
    table_state_t table;
    player_state_t players[10];
} game_state_t;

#endif