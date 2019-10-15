
// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     4210
#define MAXLINE 1025

// Driver code
int main(int argc, char** argv) {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in     servaddr;
    
    int num_frames = 1;
    int num_msgs = 32;
    char msg[MAXLINE];
    
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.175");
    
    int n, len;
    
    int f = 0;
    while(1)
    {
        for (int i = 0; i < num_msgs; i++)
        {
            msg[0] = i;
            size_t size = fread(&msg[1], 1, MAXLINE-1, stdin);
            if (size != MAXLINE-1)
            {
                exit(0);
            }
            
            sendto(sockfd, (const char*)msg, MAXLINE,
                   0, (const struct sockaddr *) &servaddr,
                   sizeof(servaddr));
        }
        printf("sent frame %d\n", f++);
        usleep(66666);
    }
     
    close(sockfd);
    return 0;
}
