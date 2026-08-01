#ifndef CLIENT_ACTIONS_H
#define CLIENT_ACTIONS_H

#define MAX_NAME 20
#define MAX_BUFF 1024


typedef struct client{
    int fd; 
    int state;  // keeping it as 0 is unregistered and 1 is registered
    char name[MAX_NAME + 1];

    int ship[5][3];
    int hits;
    
    char buff[MAX_BUFF];
    int buff_size;

    struct client *next;
}client;

client *create_client(int fd, client **head);

void remove_client(client **head, client *client);

#endif