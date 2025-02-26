#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFERSIZE 1024
#define PORT 75


int main () 
{

    struct sockaddr_in server_addr;
    struct sockaddr_in cli_addr;
    int socket_mesharet;
    int socket_mesharet_fd;

    socklen_t len;

// struct server init
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.1");
    server_addr.sin_port = htons(PORT);
    server_addr.sin_family = AF_INET;


// create udp socket

socket_mesharet = socket(AF_INET,SOCK_DGRAM,0);


if (bind(socket_mesharet,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
{
    perror("bind error");
    exit(0);
}




    return 0;
}