#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>
#define PORT 8080
#define SERVER_IP "127.0.0.1" // loop back IP address IPv4
#define BUFFER_SIZE 516
#define BLOCK_SIZE 512
#define MAXLINE 1000

// TFTP Opcodes
#define RRQ 1
#define DATA 3
#define ERROR 5
#define WRQ 2
#define ACK 4
#define TIMEOUT 3