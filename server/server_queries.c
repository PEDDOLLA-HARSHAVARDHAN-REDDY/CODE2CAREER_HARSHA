#include "server.h"
#include "server_utils.h"

// Function to handle read requests
void handle_rrq(int sockfd, struct sockaddr_in *client_addr, socklen_t len, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        send_error(sockfd, client_addr, len, 1, "File not found");
        return;
    }
    int block = 1, n;
    char buffer[BUFFER_SIZE];
    struct timeval tv;
    fd_set read_fds;
    // Send the file contents in blocks
    while ((n = fread(buffer + 4, 1, BLOCK_SIZE, fp)) > 0)
    {
        buffer[0] = 0;
        buffer[1] = DATA;
        buffer[2] = (block >> 8) & 0xFF;
        buffer[3] =
            block & 0xFF;
        int attempts = 0;
        while (attempts < MAX_RETRIES)
        {
            sendto(sockfd, buffer, n + 4, 0, (struct sockaddr *)client_addr, len);
            // Wait for acknowledgment
            FD_ZERO(&read_fds);
            FD_SET(sockfd, &read_fds);
            tv.tv_sec = TIMEOUT;
            tv.tv_usec = 0;
            int s = select(sockfd + 1, &read_fds,
                           NULL, NULL, &tv);
            if (s > 0)
            {
                char ack_buffer[BUFFER_SIZE];
                int n = recvfrom(sockfd, ack_buffer,
                                 BUFFER_SIZE, 0, (struct sockaddr *)client_addr,
                                 &len);
                if (ack_buffer[1] == ACK &&
                    (ack_buffer[2] << 8 | ack_buffer[3]) == block)
                {
                    break; // Correct ACK received
                }
            }
            else
            {
                printf("Timeout, retransmitting block %d\n", block);
                attempts++;
            }
        }
        if (attempts == MAX_RETRIES)
        {
            printf("Failed to receive ACK for block %d, giving up\n", block);
            fclose(fp);
            return;
        }
        block++;
    }
    fclose(fp);
}

// Function to handle write requests
void handle_wrq(int sockfd, struct sockaddr_in *client_addr, socklen_t len, const char *filename)
{
    int n;
    struct timeval timeout;
    timeout.tv_sec = INITIAL_TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout));

    // Check if file exists to prevent overwriting
    if (access(filename, F_OK) != -1)
    {
        send_error(sockfd, client_addr, len, 6, "File already exists.");
        return;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        send_error(sockfd, client_addr, len, 2, "Access violation.");
        return;
    }
    int block = 0;
    unsigned char buffer[BUFFER_SIZE];
    // Send ACK for block 0
    send_ack(sockfd, client_addr, len, block);

    int failCount=0;
    do
    {
        // Receive data block
        n = recvfrom(sockfd, buffer, BUFFER_SIZE,
                     0, (struct sockaddr *)client_addr, &len);
        if (n < 0)
        {
            failCount++;
            if(failCount==MAX_RETRIES){
            perror("recvfrom failed, tried multiple times");
            fclose(fp);
            return;
            }
        }else{
            failCount=0;
        }
        printf("%d\n",block);
        if (buffer[1] == DATA && (buffer[2] << 8 | buffer[3]) == block + 1)
        {
            fwrite(buffer + 4, 1, n - 4, fp); // Write data to file
            block++;
            send_ack(sockfd, client_addr, len,
                     block); // Send ACK for this block
        }
    } while (buffer[1] == DATA && (n - 4) == BLOCK_SIZE);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout));
    fclose(fp);
}