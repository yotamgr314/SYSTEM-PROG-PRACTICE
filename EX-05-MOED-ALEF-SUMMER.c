#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 8080
#define BUFSIZE 1024


int main () {

    char buffer[BUFSIZE];
    
    struct sockaddr_in serveraddr;
    struct sockaddr_in cliaddr;

    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_family = AF_INET;


    memeset(&serveraddr,0,sizeof(serveraddr));
    memeset(&cliaddr,0,sizeof(cliaddr));

    int socket_mesharet_fd =socket(AF_INET,SOCK_DGRAM,0);

    if(socket_mesharet_fd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if ( bind(socket_mesharet_fd,(const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 )
    {
        perror("failed to bind");
        exit(EXIT_FAILURE);
    }


    int forEver=1;
    socklen_t len = sizeof(cliaddr);
    
    while(forEver)
    {
    int sizeOfClientMessage = recvfrom(socket_mesharet_fd,buffer,BUFSIZE,MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);

    buffer[sizeOfClientMessage] ='\0';  // message recived saved in buffer. 


    pid_t processId = fork();

    if(processId == 0)
    {
        sendto(socket_mesharet_fd, buffer, BUFSIZE,MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len); 

        close(socket_mesharet_fd);
        exit(0);
    }else{
        if(processId < 0)
        {
            perror("fork failed");
        }
    }

    close(socket_mesharet_fd);
    return 0;

    }

    
    return 0;
}