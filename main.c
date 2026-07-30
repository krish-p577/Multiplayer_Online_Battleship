#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>


int socet_setup(int port){


    if listen_fd = socket(AF_INET, SOCK_STREAM, 0) < 0{
        perror("socket");
        exit(1);
    }
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(1);
    }

    if (listen(listen_fd, 10) < 0) {
        perror("listen");
        close(listen_fd);
        exit(1);
    }

    return listen_fd

}

void main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    if argc != 2 {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
}