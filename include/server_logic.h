#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include "server.h"
void reverse(char* pointer);

int number_to_char(char* pointer, int temporary);

int int_to_str(char *dst, ssize_t dst_size, int value);


char* create_game(int temporary_fd, char* buffer_receive, int* result_function, int* index_game, game_struct_players* game_list);

bool validate_message_length(const char* temporary_pointer);

ssize_t send_validated_message(const char* temporary_pointer, char buffer[BUFFER_SIZE], int client_file_descriptor);

ssize_t receive_validated_message(char buffer[BUFFER_SIZE], int client_file_descriptor);

void eliminate_game_slot(void* arg, int index_game, int index_player);
