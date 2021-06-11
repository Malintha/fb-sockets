#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "messages/states_generated.h"

#define PORT     28500
#define MAXLINE 1024

// Driver code
int main() {
    int sockfd;

    struct sockaddr_in servaddr;

    // create a swarm object
    flatbuffers::FlatBufferBuilder builder;
    auto position = Vec3(3.0f, 2.0f, 5.0f); 
    auto id = 1;
    auto ag1 = CreateAgent(builder, &position,id);
    std::vector<flatbuffers::Offset<Agent>> agents_vec;
    agents_vec.push_back(ag1);
    agents_vec.push_back(ag1);
    auto agents = builder.CreateVector(agents_vec);

    auto swarm = CreateSwarm(builder, agents);
    builder.Finish(swarm);

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len;

    sendto(sockfd, builder.GetBufferPointer(), builder.GetSize(),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
        sizeof(servaddr));
    printf("swarm size: %d ",builder.GetSize());


    char buffer[MAXLINE];
    ssize_t n_bytes = recvfrom(sockfd, buffer, MAXLINE, 
        MSG_WAITALL, (struct sockaddr *) &servaddr,
        &len);

    buffer[n_bytes] = '\0';
    printf("\nServer : %s\n", buffer);

    close(sockfd);
    return 0;
}