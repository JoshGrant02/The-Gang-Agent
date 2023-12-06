#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>

#define BOUNCER_NAME "/tmp/pokerbouncer"

static char thePokerTable[] = {36, 36, 36, 36, 36, 36, 36, 36, 92, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 92, 95, 95, 36, 36, 32, 32, 95, 95, 124, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 95, 95, 36, 36, 32, 32, 95, 95, 124, 32, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 92, 32, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 36, 36, 36, 36, 36, 92, 32, 32, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 92, 32, 32, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 36, 36, 36, 36, 36, 32, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 124, 32, 36, 36, 32, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 124, 32, 92, 95, 95, 95, 95, 36, 36, 92, 32, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 36, 36, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 92, 32, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 32, 32, 95, 95, 95, 95, 47, 32, 36, 36, 32, 47, 32, 32, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 32, 32, 47, 32, 36, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 32, 124, 32, 32, 92, 95, 95, 124, 36, 36, 32, 124, 32, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 36, 36, 32, 124, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 32, 32, 95, 95, 95, 95, 124, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 32, 95, 36, 36, 60, 32, 32, 36, 36, 32, 32, 32, 95, 95, 95, 95, 124, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 36, 36, 32, 32, 95, 95, 36, 36, 32, 124, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 36, 36, 32, 124, 36, 36, 32, 32, 32, 95, 95, 95, 95, 124, 10, 32, 32, 32, 36, 36, 32, 124, 32, 32, 32, 36, 36, 32, 124, 32, 32, 36, 36, 32, 124, 92, 36, 36, 36, 36, 36, 36, 36, 92, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 92, 36, 36, 36, 36, 36, 36, 32, 32, 124, 36, 36, 32, 124, 32, 92, 36, 36, 92, 32, 92, 36, 36, 36, 36, 36, 36, 36, 92, 32, 36, 36, 32, 124, 32, 32, 32, 32, 32, 32, 36, 36, 32, 124, 92, 36, 36, 36, 36, 36, 36, 36, 32, 124, 36, 36, 36, 36, 36, 36, 36, 32, 32, 124, 36, 36, 32, 124, 92, 36, 36, 36, 36, 36, 36, 36, 92, 32, 10, 32, 32, 32, 92, 95, 95, 124, 32, 32, 32, 92, 95, 95, 124, 32, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124, 92, 95, 95, 124, 32, 32, 32, 32, 32, 32, 32, 92, 95, 95, 95, 95, 95, 95, 47, 32, 92, 95, 95, 124, 32, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124, 92, 95, 95, 124, 32, 32, 32, 32, 32, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124, 92, 95, 95, 95, 95, 95, 95, 95, 47, 32, 92, 95, 95, 124, 32, 92, 95, 95, 95, 95, 95, 95, 95, 124};
pthread_mutex_t consoleMutex;

void* bouncer_entry(void* param);
void* player_thread(void* player_param);

int main(int argc, char** argv)
{
    printf("Welcome to\n%s\n", thePokerTable);

    pthread_mutex_init(&consoleMutex, NULL);

    pthread_t bouncer;
    pthread_create(&bouncer, NULL, bouncer_entry, (void*)0);

    for (int i = 0; i < 50; i++)
    {
        usleep(20000000);
        pthread_mutex_lock(&consoleMutex);
        printf("waiting...\n");
        pthread_mutex_unlock(&consoleMutex);
    }

    pthread_join(bouncer, NULL);
}

void* bouncer_entry(void* param)
{
    int listener = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(BOUNCER_NAME);
    
    struct sockaddr_un listener_address;
    listener_address.sun_family = AF_UNIX;
    strcpy(listener_address.sun_path, BOUNCER_NAME);

    bind(listener, (struct sockaddr*) &listener_address, sizeof(struct sockaddr_un));
    listen(listener, 10);

    pthread_mutex_lock(&consoleMutex);
    printf("BOUNCER: I'm waiting for players\n");
    pthread_mutex_unlock(&consoleMutex);

    pthread_t players[10];
    int num_players = 0;

    int player;
    while(num_players < 3)
    {
        socklen_t listener_size = sizeof(struct sockaddr_un);
        player = accept(listener, (struct sockaddr*) &listener_address, &listener_size);
        pthread_create(&players[num_players], NULL, player_thread, (void*) player);

        num_players++;
    }
    pthread_join(players[0], NULL);
    printf("I'm done waiting for people\n");
}

void* player_thread(void* player_param)
{
    printf("I found someone\n");
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
        pthread_mutex_lock(&consoleMutex);
        printf("recieving message from %d: %s\n", player, message);
        pthread_mutex_unlock(&consoleMutex);
    }
    pthread_mutex_lock(&consoleMutex);
    printf("Player is quitting\n");
    pthread_mutex_unlock(&consoleMutex);
    close(player);
}