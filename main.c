#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>
#include <signal.h>
// #include <sys/socket.h>
// #include <netinet/in.h>

#include "client_mgr.h"
#include "game_logic.h"


int socket_setup(int port){


    int listen_fd;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    return listen_fd;

}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);
    int listen_fd = socket_setup(port);
    printf("Starting server on port %d...\n", port);

    fd_set read_fds;
    int max_fd;

    while(1){
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);
        max_fd = listen_fd;
        
        client *curr = head;
        while(curr != NULL){
            FD_SET(curr->fd, &read_fds);
            if(curr->fd > max_fd){
                max_fd = curr->fd;
            }
            curr = curr->next;
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(listen_fd, &read_fds)) {
            int new_fd = accept(listen_fd, NULL, NULL);
            if (new_fd >= 0) {
                add_client(new_fd);
            }
        }

        curr = client_list;
        while(curr != NULL){
            client *next_client = curr->next;

            if(FD_ISSET(curr->fd, &read_fds))
            

        }


    close(listen_fd);
    return 0;
}