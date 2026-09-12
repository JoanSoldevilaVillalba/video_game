#ifndef TEST_SERVER_INTERFACE_H
#define TEST_SERVER_INTERFACE_H

#include "test_server_main.h"

typedef enum {
    RANDOM_MESSAGE_TEST = 0,
    QUIT_CLIENT_MESSAGE_TEST = 1,
    FOUND_GAME_TEST = 2,
    CREATED_GAME_TEST = 3,
    GAMES_OCCUPIED_TEST = 4,
    GAME_EXPERATION_TEST = 5,
    GAME_NO_SCND_PLAYER_TEST = 6,
    MENU_PREPERATION_INFO = 7,
    MENU_PREPERATION_FAIL_GI = 8,
    MENU_PREPERATION_FAIL_PI = 9,
    MAX_MESSAGES_TEST = 10
} message_server_id;

extern const char* test_message_server[];

int test_menu_information_in_game(char* buffer_message, char* buffer_error, int file_descriptor);
int test_menu_information_not_in_game(char* buffer_message, char* buffer_error, int file_descriptor);
int test_quit_client(char* buffer_message, char* buffer_error, int file_descriptor);
int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor);
int test_setup_connection(char* buffer_error, int server_port);
int test_create_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor);

#endif // TEST_SERVER_INTERFACE_H
