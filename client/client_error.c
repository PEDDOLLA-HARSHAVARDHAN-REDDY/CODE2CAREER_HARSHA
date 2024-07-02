#include "client.h"
// Function to send an error packet
void send_error(int sockfd, struct sockaddr_in *server_addr, socklen_t len, int error_code, const char *message)
{
    char buffer[BUFFER_SIZE];
    int message_len = strlen(message);
    buffer[0] = 0;
    buffer[1] = ERROR;
    buffer[2] = (error_code >> 8) & 0xFF;
    buffer[3] =
        error_code & 0xFF;
    strcpy(buffer + 4, message);
    buffer[message_len + 4] = 0; // Null terminate the error message
    sendto(sockfd, buffer, message_len + 5, 0, (struct sockaddr *)server_addr, len);
    printf("Error sent: %s\n", message);
}