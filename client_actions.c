#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_actions.h"

client *create_client(int fd, client **head) {
    client *new_client = (client *)malloc(sizeof(client));
    if (new_client == NULL) {
        perror("malloc");
        return NULL;
    }
    new_client->fd = fd;
    new_client->state = 0;

    new_client->buff_size = 0;
    memset(new_client->buff, 0, MAX_BUFF);
    memset(new_client->name, 0, MAX_NAME);

    new_client->hits = 0;
    for (int i = 0; i < 5; i++) {
        new_client->ship[i][2] = 0; // Initialize ship hits to 0
    }
    new_client->next = *head;
    *head = new_client;
    return new_client;
}

void remove_client(client **head, client *curr_client) {
    char msg[100];
    snprintf(msg, sizeof(msg), "GG %s\n", curr_client->name);

    send_message(*head, msg);

    close(curr_client->fd);

    if(*head == curr_client) {
        *head = curr_client->next;
    }else{
        client *curr = *head;
        while(curr != NULL && curr->next != curr_client) {
            curr = curr->next;
        }
        if(curr != NULL) {
            curr->next = curr_client->next;
        }
    }
    free(curr_client);
}