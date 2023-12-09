#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handCalculator.h"

#define BOUNCER_NAME "/tmp/pokerbouncer"
#define CLIENT_NAME "/tmp/player"

void printHelp();
void* receiver(void* param);

int main(int argc, char** argv)
{
    int sock;

    if ((sock = socket( AF_UNIX, SOCK_STREAM, 0 )) < 0)
    {
        perror("Socket failed to create");
        exit(1);
    }

    struct sockaddr_un sock_address;

    sock_address.sun_family = AF_UNIX;
    strcpy(sock_address.sun_path,CLIENT_NAME);

    // our little "unlink trick"
    unlink(CLIENT_NAME);

    // we must now bind the socket descriptor to the address info
    if (bind(sock, (struct sockaddr *) &sock_address, sizeof(sock_address))<0)
    {
        printf("Socket failed to bind\n");
        exit(1);
    }

    // Need to address server with same technique
    struct sockaddr_un server;

    // address family is AF_INET
    // fill in INADDR_ANY for address (any of our IP addresses)
    // for a client, this would be the desitation address
    // the port number is per default or option above
    // note that address and port must be in memory in network order

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,BOUNCER_NAME);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("The Poker Table is currently not operating. Try again later\n");
        exit(1);
    }
    
    char buffer[sizeof(int)];

    while(1)
    {
        recv(sock, buffer, sizeof(int), 0);
        int card = 0;
        for (size_t i = 0; i < sizeof(int); ++i)
        {
            card |= buffer[i] << (8 * i);
        }
        if (card == -1)
        {
            printf("The table is quitting :( I am leaving\n");
            break;
        }
        char valueBuffer[3];
        getCardValue(card, valueBuffer);
        printf("I just got card %s\n", valueBuffer);
    }
    
    /*
    //char buffer[100];
    int size_to_send;
    int size_sent;
    int size_echoed;

    buffer[0] = 0;

    while (strcmp(buffer, "quit\n") != 0)
    {
        printf("Message: ");
        
        // get a line
        fgets(buffer, 100, stdin);

        if (strcmp(buffer, "help\n") == 0)
        {
            printHelp();
        }
        else
        {
            printf("Sending Message: %s", buffer);
            // how big?
            size_to_send = strlen(buffer);

            // send to server
            size_sent = send(sock, buffer, size_to_send, 0);
            if (size_sent < 0) {
                perror("Send error");
            }
        }
    }
    */

    close(sock);
    unlink(CLIENT_NAME);
    return 0;
}

void printHelp()
{
    printf("FOR THE PURPOSE OF THE CPE 2600 LAB, NONE OF THESE ARE IMPLEMENTED\n");
    printf("Below is a list of all commands that you can run. They are split into lobby, table, and turn commands:\n");
    printf("\nLobby Commands: These commands can only be ran when you are in the lobby\n");
    printf("\thelp - Print this help message\n");
    printf("\tquit - Quit the game\n");
    printf("\tcheckBalance - Print your current chip balance to the console\n");
    printf("\tqueueEnter - Queue to enter the table once the current hand is over\n");
    printf("\tbuyIn <value> - Add <value> to your current chip balance\n");

    printf("\nTable Commands: These commands can be run anytime while at the table\n");
    printf("\thelp - Print this help message\n");
    printf("\tcheckBalance - Print your current chip balance to the console\n");
    printf("\tqueueLeave - Queue to leave the table once the current hand is over\n");

    printf("\nTurn Commands: These commands can only be run when it is your turn at the table\n");
    printf("\tbet <value> - Bet the specified value. <value> must be >= current bet\n");
    printf("\traise <value> - Raise the bet by the specified value\n");
    printf("\tcheck - Check your turn. Can only be ran if current bet == 0\n");
    printf("\tfold - Fold your hand\n");
    printf("\tall in - Equivalent to bet <chip balance>. Can be ran if chip balance < current bet\n\n");
}