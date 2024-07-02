void send_rrq(char *buf, int sockfd, const char *filename, const char *mode, struct sockaddr_in *servaddrptr);
void send_wrq(char *buf, int sockfd, const char *filename, const char *mode, struct sockaddr_in *servaddrptr);
void handle_wrq(int sockfd, struct sockaddr_in *server_addr, socklen_t len,const char *filename);
void send_ack(int sockfd, struct sockaddr_in *client_adr, socklen_t len, int block);
void receive_rrq_data(int sockfd, struct sockaddr_in *server_addr, socklen_t len,const char *filename);
void send_error(int sockfd, struct sockaddr_in *client_addr, socklen_t len, int error_code, const char *message);