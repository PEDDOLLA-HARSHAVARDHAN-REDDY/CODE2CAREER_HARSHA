#include "server.h"
#include "server_utils.h"

// Function to handle read requests
int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUFFER_SIZE];
    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Waiting for request...\n");
        len = sizeof(cliaddr);
        // Receive RRQ from client
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE,
                         0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
        {
            perror("recvfrom timed out or failed");
            continue;
        }

        int opcode = buffer[1];
        char *filename = buffer + 2;

        switch (opcode)
        {
        case RRQ:
        {
            printf("RRQ received. Processing...\n");
            handle_rrq(sockfd, &cliaddr, len, filename);
            break;
        }
        case WRQ:
        {
            printf("WRQ received for file: %s\n", filename);
            handle_wrq(sockfd, &cliaddr, len, filename);
            break;
        }
        default:
        {
            send_error(sockfd, &cliaddr, len, 4, "Illegal TFTP operation.");
            break;
        }
        }
    }
    // Close the socket
    close(sockfd);
    return 0;
}