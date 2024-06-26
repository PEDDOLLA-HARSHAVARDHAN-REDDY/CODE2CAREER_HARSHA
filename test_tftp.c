// test_tftp.c
#include <assert.h>
#include "client.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXLINE 1000

int setup_mock_socket(){
    int sockfd;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}
void test_send_rrq()
{
    // Setup a mock server and client
    int sockfd = setup_mock_socket();
    const char *filename = "testfile.txt"; // Send a read request from the client
    send_rrq(sockfd, filename);
    // Verify that the server receives the request
    char buffer[MAXLINE];
    int n = recv(sockfd, buffer, MAXLINE, 0);
    assert(n > 0 && buffer[1] == 1); // Check if the opcode is RRQ
}
int main()
{
    test_send_rrq();
    printf("All tests passed.\n");
    return 0;
}