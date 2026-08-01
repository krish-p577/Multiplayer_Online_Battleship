#include "client_actions.h"

#ifndef GAME_ACTIONS_H
#define GAME_ACTIONS_H

void process_message(client **head, client *curr_client, char *msg);

void send_exact(client **head, client *curr_client, char *msg);

void send_message(client **head, char *msg);

#endif