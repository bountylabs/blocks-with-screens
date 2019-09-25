
// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     8080
#define MAXLINE 1025

// Driver code
int main(int argc, char** argv) {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in     servaddr;
    
    int num_frames = 1;
    int num_msgs = 1;
    char msg[MAXLINE];
    
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(4210);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.31");
    
    int n, len;
    
    for (int f = 0; f < num_frames; f++)
    {
        for (int i = 0; i < num_msgs; i++)
        {
            msg[0] = i;
            
            fread(msg, 1, MAXLINE, stdin);
            
            sendto(sockfd, (const char*)msg, MAXLINE,
                   0, (const struct sockaddr *) &servaddr,
                   sizeof(servaddr));
            //printf("sent message %d\n", i);
        }
        printf("sent frame %d\n", f);
        
        usleep(50000);
    }
     
    close(sockfd);
    return 0;
}
