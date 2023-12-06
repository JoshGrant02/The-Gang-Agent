#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOUNCER_NAME "/tmp/pokerbouncer"
#define CLIENT_NAME "/tmp/player"

int main(int argc, char** argv)
{
    printf("I am a player\n");

    int sock;

    if ((sock = socket( AF_UNIX, SOCK_STREAM, 0 )) < 0)
    {
        perror("Error creating socket");
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
        printf("Gosh darn it\n");
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
        perror("Connection error");
        exit(1);
    }

    // ready to send
    printf("Ready to send\n");

    char buffer[100];
    int size_to_send;
    int size_sent;
    int size_echoed;

    while (strcmp(buffer, "quit\n") != 0)
    {
        printf("Message: ");
        
        // get a line
        fgets(buffer, 100, stdin);

        printf("Sending Message: %s", buffer);
        // how big?
        size_to_send = strlen(buffer);

        // send to server
        size_sent = send(sock, buffer, size_to_send, 0);
        if (size_sent < 0) {
            perror("Send error");
        }
    }

    close(sock);
    unlink(CLIENT_NAME);
}