#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_actions.h"
#include "game_actions.h"

void send_message(client **head, client *curr_client, char *msg) {
    int len = strlen(msg);
    int written = write(curr_client->fd, msg, len);
    if (written < len) {
        remove_client(head, curr_client);
    }
}

void process_message(client **head, client *sender, char *msg){

    if (strncmp(msg, "REG ", 4) == 0) {
        char name[100];
        int x, y;
        char d;
        
        if (sscanf(msg, "REG %20s %d %d %c", name, &x, &y, &d) == 4) {
            
            
            int valid_name = 1;
            for (int i = 0; name[i] != '\0'; i++) {
                if (!isalnum(name[i]) && name[i] != '-') {
                    valid_name = 0;
                    break;
                }
            }
        
            int valid_bounds = 0;
            int temp_cells[5][3];
            
            if (d == '-' && (x - 2 >= 0) && (x + 2 <= 9) && y >= 0 && y <= 9) {
            
                for (int i = 0; i < 5; i++) {
                    temp_cells[i][0] = x - 2 + i;
                    temp_cells[i][1] = y;
                }
                valid_bounds = 1;
            } else if (d == '|' && x >= 0 && x <= 9 && (y - 2 >= 0) && (y + 2 <= 9)) {
                
                for (int i = 0; i < 5; i++) {
                    temp_cells[i][0] = x;
                    temp_cells[i][1] = y - 2 + i;
                }
                valid_bounds = 1;
            }

            if (!valid_name || !valid_bounds) {
                send_exact(sender->fd, "INVALID\n");
                return;
            }

            
            client *curr = *head;
            while (curr != NULL) {
                if (curr->state == 1 && strcmp(curr->name, name) == 0) {
                    send_exact(sender->fd, "TAKEN\n");
                    return;
                }
                curr = curr->next;
            }

            
            strcpy(sender->name, name);
            memcpy(sender->ship, temp_cells, sizeof(temp_cells));

            
            sender->state = 1;
            
            send_exact(sender->fd, "WELCOME\n");
            
            
            char join_msg[100];
            snprintf(join_msg, sizeof(join_msg), "JOIN %s\n", sender->name);
            broadcast(*head, join_msg);
            
        } else {
            send_exact(sender->fd, "INVALID\n");
        }
    } 

    else if (strncmp(msg, "BOMB ", 5) == 0) {
        int tx, ty;
        
        if (sender->state == 1 && sscanf(msg, "BOMB %d %d", &tx, &ty) == 2) {
            
            int hit_anything = 0;
            client *curr = *head;
            
            
            while (curr != NULL) {
                client *next_client = curr->next;
                
                if (curr->state == 1) {
    
                    for (int i = 0; i < 5; i++) {
                        if (curr->ship[i][0] == tx && curr->ship[i][1] == ty) {
                            hit_anything = 1;
                            
                            
                            if (curr->ship[i][2] == 0) {
                                curr->ship[i][2] = 1;
                                curr->hits++;
                            }
                            
                            
                            char hit_msg[128];
                            snprintf(hit_msg, sizeof(hit_msg), "HIT %s %d %d %s\n", sender->name, tx, ty, curr->name);
                            broadcast(*head, hit_msg);
                            
                            if (curr->hits == 5) {

                                remove_client(head, curr);
                                break; 
                            }
                        }
                    }
                }
                curr = next_client;
            }

    
            if (!hit_anything) {
                char miss_msg[128];
                snprintf(miss_msg, sizeof(miss_msg), "MISS %s %d %d\n", sender->name, tx, ty);
                broadcast(*head, miss_msg);
            }
            
        } else {
            send_exact(sender->fd, "INVALID\n");
        }
    } 

    else {
        send_exact(sender->fd, "INVALID\n");
    }
}