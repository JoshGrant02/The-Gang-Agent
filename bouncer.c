#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include "playerServant.h"

#define BOUNCER_NAME "/tmp/pokerbouncer"

void* bouncer_entry(void* param)
{
    int listener = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(BOUNCER_NAME);
    
    struct sockaddr_un listener_address;
    listener_address.sun_family = AF_UNIX;
    strcpy(listener_address.sun_path, BOUNCER_NAME);

    bind(listener, (struct sockaddr*) &listener_address, sizeof(struct sockaddr_un));
    listen(listener, 10);

    //pthread_mutex_lock(&consoleMutex);
    printf("BOUNCER: I'm waiting for players\n");
    //pthread_mutex_unlock(&consoleMutex);

    pthread_t players[10];
    int num_players = 0;

    int player;
    while(num_players < 2)
    {
        socklen_t listener_size = sizeof(struct sockaddr_un);
        player = accept(listener, (struct sockaddr*) &listener_address, &listener_size);
        pthread_create(&players[num_players], NULL, player_servant, (void*) player);

        num_players++;
    }
    pthread_join(players[0], NULL);
    num_players++;
}