#include "client.h"
// Function to send ACK
void send_ack(int sockfd, struct sockaddr_in *server_adr, socklen_t len, int block)
{
    char buffer[4];
    buffer[0] = 0;
    buffer[1] = ACK;
    buffer[2] = (block >> 8) & 0xFF;
    buffer[3] =
        block & 0xFF;
    sendto(sockfd, buffer, 4, 0, (struct sockaddr *)server_adr, len);
}