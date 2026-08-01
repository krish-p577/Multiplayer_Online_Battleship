#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>
#include <signal.h>
// #include <sys/socket.h>
// #include <netinet/in.h>

#include "client_actions.h"
#include "game_actions.h"


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

    client *client_list = NULL; 

    printf("Starting server on port %d...\n", port);

    fd_set read_fds;
    int max_fd;

    while(1){
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);
        max_fd = listen_fd;
        
        client *curr = client_list; 
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
                create_client(&client_list, new_fd);
            }
        }

        curr = client_list;
        while(curr != NULL){
            client *next_client = curr->next;

            if(FD_ISSET(curr->fd, &read_fds)){
                int space_left = MAX_BUFF - curr->buff_size;
                int bytes_read = read(curr->fd, curr->buff + curr->buff_size, space_left, 0);
                
                if(bytes_read <= 0){
                    remove_client(&client_list, curr);
                } else {
                    curr->buff_size += bytes_read;
                    
                    int line_start = 0;
                    for(int i = 0; i < curr->buff_size; i++){
                        if(curr->buff[i] == '\n'){
                            curr->buff[i] = '\0';
                            process_message(&client_list, curr, curr->buff + line_start);
                            line_start = i + 1;
                        }
                    }

                    if(line_start > 0){
                        int remaining = curr->buff_size - line_start;
                        memmove(curr->buff, curr->buff + line_start, remaining);
                        curr->buff_size = remaining;
                    }else if(curr->buff_size >= 100){
                        remove_client(&client_list, curr);
                    }
                }
            }
            curr = next_client;

        }

    }
    close(listen_fd);
    return 0;
    
}