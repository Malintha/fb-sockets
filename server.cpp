#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "messages/states_generated.h"

#define PORT 28500
#define MAXLINE 1024

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;


    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
}

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
}

    socklen_t len, n;

    len = sizeof(cliaddr); 
    char buffer[MAXLINE];
    ssize_t n_bytes = recvfrom(sockfd, buffer, MAXLINE, 
        MSG_WAITALL, ( struct sockaddr *) &cliaddr,
        &len);

    // flatbuffers::Verifier verifier(recv_buffer, static_cast<size_t>(n_bytes));

    // buffer[n] = '\0';
    char data[n_bytes];
    std::memcpy(data, buffer, sizeof(data));
    auto swarm = GetSwarm(data);
    auto agents = swarm->agents();
    
    printf("agents : %d\n", agents->size());
    const Vec3 *v = agents->Get(0)->position();
    printf("pos : %f %f %f \n", v->x(), v->y(),v->z());

    sendto(sockfd, (const char *)("processed data"), strlen("processed data"), 
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
        len);

    printf("processed data sent.\n"); 

    return 0;
}