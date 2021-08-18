#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include "messages/states_generated.h"

#define PORT     28500
#define MAXLINE 1024

// Driver code
int main() {
    int sockfd;

    struct sockaddr_in servaddr;

    // create a swarm object
    flatbuffers::FlatBufferBuilder builder;
    auto position = Vec3(-20.0f, -10.0f, 5.0f); 
    auto id = 1;
    auto ag1 = CreateAgent(builder, &position,id);

    position = Vec3(5.0f, -12.0f, 5.0f);     
    id = 2;
    auto ag2 = CreateAgent(builder, &position,id);

    position = Vec3(0.0f,-20.0f, 5.0f);
    id = 3;
    auto ag3 = CreateAgent(builder, &position, id);

    std::vector<flatbuffers::Offset<Agent>> agents_vec;
    agents_vec.push_back(ag1);
    agents_vec.push_back(ag2);
    agents_vec.push_back(ag3);
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
    printf("swarm size: %d \n",builder.GetSize());


    char buffer[MAXLINE];
    ssize_t n_bytes = recvfrom(sockfd, buffer, MAXLINE, 
        MSG_WAITALL, (struct sockaddr *) &servaddr,
        &len);

    // buffer[n_bytes] = '\0';

    char agent_data[n_bytes];
    std::memcpy(agent_data, buffer, sizeof(agent_data));

    // printf("\nServer : %d\n", );
    printf("recieved: %d \n", sizeof(agent_data));
    close(sockfd);
    return 0;
}
