#include "server.h"
// Function to send ACK
void send_ack(int sockfd, struct sockaddr_in *client_adr, socklen_t len, int block)
{
    unsigned char buffer[4];
    buffer[0] = 0;
    buffer[1] = ACK;
    buffer[2] = (block >> 8) & 0xFF;
    buffer[3] =
        block & 0xFF;
        //printf("sent ack %d\n",buffer[2]<<8 | buffer[3]);
    sendto(sockfd, buffer, 4, 0, (struct sockaddr *)client_adr, len);
}