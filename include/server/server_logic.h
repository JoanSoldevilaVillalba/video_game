#ifndef SERVER_LOGIC_H
#define SERVER_LOGIC_H

#include <stdbool.h>
#include <sys/types.h>
#include "server_send_data.h"
#include "server.h"

void time_init(struct timespec* ts, int time_experation);

int str_to_int(char* buffer_message, char* buffer_error);

void reverse(char* pointer);

int number_to_char(char* pointer, int temporary);

int int_to_str(char *dst, ssize_t dst_size, int value);



bool validate_message_length(char* buffer_message,char* buffer_error);

bool validate_message_structure(char* buffer_message,char* buffer_error);

bool validate_message_numbers(char* buffer_message, char* buffer_error);


ssize_t send_validated_message(char* buffer_message,char* buffer_error, int client_file_descriptor);

ssize_t receive_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor);


void switch_game_player_position(game_struct_players* list_game_pointer, int* index_player);

void eliminate_game_slot(void* arg, int* index_game, int* index_player);

int wait_signal_cond(game_struct_players * list_game_pointer, int index_player, struct timespec* ts, int time_exp);

void waiting_for_player(struct_client* client, int* index_game,int* index_player, int time_experation, struct timespec* ts, int* counter,char buffer_receive[],  int* result_function, int* timed_out, char* buffer_message);

void handlePE(ssize_t* result, char* buffer_receive,char* buffer_error, bool* quit, int* first_number);

void create_game(int temporary_fd, int* result_function, int* index_game, game_struct_players* game_list, char* buffer_message);

int menu_preperation_validation(struct_client* client, int index, char* temporary_buffer, char* buffer_error);


#endif //SERVER_LOGIC_H
