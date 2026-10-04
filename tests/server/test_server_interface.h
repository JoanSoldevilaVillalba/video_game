#ifndef TEST_SERVER_INTERFACE_H
#define TEST_SERVER_INTERFACE_H

#include "test_server_main.h"

typedef enum {
    RANDOM_MESSAGE_TEST,
    QUIT_CLIENT_MESSAGE_TEST,
    FOUND_GAME_TEST,
    CREATED_GAME_TEST,
    GAMES_OCCUPIED_TEST,
    GAME_EXPERATION_TEST,
    GAME_NO_SCND_PLAYER_TEST,
    MENU_PREPERATION_INFO,
    MENU_PREPERATION_FAIL_GI,
    MENU_PREPERATION_FAIL_PI,
    NO_CREATED_GAME_WAIT,
    SCND_PL_FOUND_WATT,
    SCND_PL_NOT_FOUND_WAIT,
    MAX_MESSAGES_TEST
} message_server_id;

extern const char* test_message_server[];

int test_menu_information_in_game(char* buffer_message, char* buffer_error, int file_descriptor);
int test_menu_information_not_in_game(char* buffer_message, char* buffer_error, int file_descriptor);
int test_quit_client(char* buffer_message, char* buffer_error, int file_descriptor);
int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor);
int test_setup_connection(char* buffer_error, int server_port);
int test_create_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_enter_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_wait_second_player_init(char*buffer_message,char* buffer_error,int client_file_descriptor);
#endif // TEST_SERVER_INTERFACE_H
