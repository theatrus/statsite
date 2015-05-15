#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char** argv) {
    int sock = 0;
    char send_data[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s address port\n", argv[0]);
        exit(1);
    }
    
    host = gethostbyname(argv[1]);
    int port = atoi(argv[2]);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);

    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(struct sockaddr)) == -1) {
        perror("Connect");
        exit(1);
    }

    while(1) {
            send(sock,send_data,strlen(send_data), 0);
    }
    return 0;
}
