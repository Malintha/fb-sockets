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
#include "messages/neighborhoods_generated.h"

#define PORT     28500
#define MAXLINE 1024

// Driver code
int main() {
    int sockfd;

    struct sockaddr_in servaddr;

    // create a swarm object
    flatbuffers::FlatBufferBuilder builder;
    auto position = Vec3(-20.0f, -10.0f, 5.0f); 
    auto id = 0;
    auto ag0 = CreateAgent(builder, &position,id);

    position = Vec3(5.0f, -12.0f, 5.0f);     
    id = 1;
    auto ag1 = CreateAgent(builder, &position,id);

    position = Vec3(0.0f,-20.0f, 5.0f);
    id = 2;
    auto ag2 = CreateAgent(builder, &position, id);

    position = Vec3(14.0f, 8.0f, 0.0f);
    id = 3;
    auto ag3 = CreateAgent(builder, &position,id);

    position = Vec3(23.0f,-20.0f, 0.0f);    
    id = 4;
    auto ag4 = CreateAgent(builder, &position,id);

    position = Vec3(-25.0f,-20.0f, 0.0f);
    id = 5;
    auto ag5 = CreateAgent(builder, &position, id);

    position = Vec3(0.0f,3.0f, 0.0f);
    id = 6;
    auto ag6 = CreateAgent(builder, &position,id);

    std::vector<flatbuffers::Offset<Agent>> agents_vec;
    agents_vec.push_back(ag0);
    agents_vec.push_back(ag1);
    agents_vec.push_back(ag2);
    agents_vec.push_back(ag3);
    agents_vec.push_back(ag4);
    agents_vec.push_back(ag5);
    agents_vec.push_back(ag6);

    auto agents = builder.CreateVector(agents_vec);

    auto swarm = CreateSwarm(builder, 3, agents);
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

    // deserialize recieved data
    auto neighborhoods_temp = GetNeighborhoods(agent_data);
    auto neighborhoods = neighborhoods_temp->neighborhood();

    for(int i=0;i<neighborhoods->size(); i++) {
        printf("id: %d \t",neighborhoods->Get(i)->id());
        const auto neighbors = neighborhoods->Get(i)->neighbors();
        for(int j=0; j<neighbors->size(); j++) {
            // auto id = neighbors->Get(j);
            printf("%d ", neighbors->Get(j));
        }
        printf("\n");
    }

    close(sockfd);
    return 0;
}
