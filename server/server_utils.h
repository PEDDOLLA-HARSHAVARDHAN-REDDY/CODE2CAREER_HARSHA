#include "server.h"
void send_ack(int sockfd, struct sockaddr_in *client_addr, socklen_t len, int block);
void handle_wrq(int sockfd, struct sockaddr_in *client_addr, socklen_t len, const char *filename);
void send_error(int sockfd, struct sockaddr_in *client_addr, socklen_t len, int error_code, const char *message);
void handle_rrq(int sockfd, struct sockaddr_in *client_addr, socklen_t len, const char *filename);