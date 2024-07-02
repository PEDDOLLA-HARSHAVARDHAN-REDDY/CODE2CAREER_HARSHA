#include"client.h"
#include"client_utils.h"

int main(int argc,char *argv[])
{

    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if(!strcmp(argv[1],"RRQ")){
    send_rrq(buffer, sockfd, argv[2], "O", &servaddr);
    receive_rrq_data(sockfd,&servaddr,sizeof(servaddr),argv[3]);
    }else if(!strcmp(argv[1],"WRQ")){
    send_wrq(buffer, sockfd, argv[2], "O", &servaddr);
    handle_wrq(sockfd,&servaddr,sizeof(servaddr),argv[3]);
    }else{
        perror("incorrect arguments\n");
        exit(EXIT_FAILURE);
    }

    // Close the socket
    close(sockfd);
    return 0;
}
