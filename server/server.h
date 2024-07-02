#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#define PORT 8080
#define BUFFER_SIZE 516
#define BLOCK_SIZE 512
#define TIMEOUT 3
// TFTP Opcodes
#define RRQ 1
#define DATA 3
#define ACK 4
#define ERROR 5
#define WRQ 2