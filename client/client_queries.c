#include"client.h"
#include"client_utils.h"

void send_rrq(char *buf, int sockfd, const char *filename, const char *mode, struct sockaddr_in *servaddrptr)
{
    size_t filename_size = strlen(filename);
    size_t mode_size = strlen(mode);
    buf[0] = 0;
    buf[1] = RRQ;
    strcpy(buf + 2, filename);
    buf[filename_size + 2] = 0;
    strcpy(buf + filename_size + 3, mode);
    buf[filename_size + 3 + mode_size] = 0;
    size_t sent = sendto(sockfd, buf, (2+filename_size+1+mode_size+1), 0, (const struct sockaddr *)servaddrptr, sizeof(*servaddrptr));
    printf("RRQ sent\n");
}

void receive_rrq_data(int sockfd, struct sockaddr_in *server_addr, socklen_t len,const char *filename){
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("unbale to write received data to a file");
        return;
    }

    int block = 0, n;
    char buffer[BUFFER_SIZE];
    struct timeval tv;
    fd_set write_fds;
    
    do
    {
        // Receive data block
        n = recvfrom(sockfd, buffer, BUFFER_SIZE,
                     0, (struct sockaddr *)server_addr, &len);
        if (n < 0)
        {
            perror("recvfrom failed");
            fclose(fp);
            return;
        }
        if (buffer[1] == DATA && (buffer[2] << 8 | buffer[3]) == block + 1)
        {
            fwrite(buffer + 4, 1, n - 4, fp); // Write data to file
            block++;
            send_ack(sockfd,server_addr,len,block); // Send ACK for this block
        }
    } while (buffer[1] == DATA && (n - 4) == BLOCK_SIZE);
    fclose(fp);  
}

void send_wrq(char *buf, int sockfd, const char *filename, const char *mode, struct sockaddr_in *servaddrptr)
{
    size_t filename_size = strlen(filename);
    size_t mode_size = strlen(mode);
    buf[0] = 0;
    buf[1] = WRQ;
    strcpy(buf + 2, filename);
    buf[filename_size + 2] = 0;
    strcpy(buf + filename_size + 3, mode);
    buf[filename_size + 3 + mode_size] = 0;
    size_t sent = sendto(sockfd, buf, (2+filename_size+1+mode_size+1), 0, (const struct sockaddr *)servaddrptr, sizeof(*servaddrptr));
    printf("WRQ sent\n");
}

void handle_wrq(int sockfd, struct sockaddr_in *server_addr, socklen_t len, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        send_error(sockfd, server_addr, len, 1,"File not found");
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
        while (attempts < 5)
        {
            sendto(sockfd, buffer, n + 4, 0, (struct sockaddr *)server_addr, len);
            // Wait for acknowledgment
            FD_ZERO(&read_fds);
            FD_SET(sockfd, &read_fds);
            tv.tv_sec = TIMEOUT;
            tv.tv_usec = 0;
            int s = select(sockfd + 1, &read_fds,
                           NULL, NULL, &tv);
            if (s > 0)
            {
                unsigned char ack_buffer[BUFFER_SIZE];
                recvfrom(sockfd, ack_buffer,
                         BUFFER_SIZE, 0, (struct sockaddr *)server_addr,
                         &len);
                if (ack_buffer[1] == ACK &&
                    ((ack_buffer[2] << 8) | ack_buffer[3]) == block)
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
        if (attempts == 5)
        {
            printf("Failed to receive ACK for block %d, giving up\n", block);
            fclose(fp);
            return;
        }
        block++;
    }
    fclose(fp);
}