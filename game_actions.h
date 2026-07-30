#include "client_actions.h"

#ifndef GAME_ACTIONS_H
#define GAME_ACTIONS_H

int place_ship(int x, int y, int ship_arr[5][3]);

int validate_name(char *name);

void process_register(int fd, char *name, client *head, int *state);

void process_attack(int fd, char *name, int x, int y, client *head);

void send_exact(int fd, char *msg);

void send_message(client *head, char *msg);


#endif