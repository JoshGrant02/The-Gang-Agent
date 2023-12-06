#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include "playerServant.h"

void* player_servant(void* player_param)
{
    printf("I found someone\n");
    int player = (int) player_param;
    char buffer[100];
    recv(player, buffer, 100, 0);


    //pthread_mutex_lock(&consoleMutex);
    printf("recieving message: %s\n", buffer);
    //pthread_mutex_unlock(&consoleMutex);
    close(player);
}