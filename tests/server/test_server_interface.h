#ifndef TEST_SERVER_INTERFACE_H
#define TEST_SERVER_INTERFACE_H

#include "test_server_main.h"

typedef enum
{
    RANDOM_MESSAGE_TEST,        // 0
    QUIT_CLIENT_MESSAGE_TEST,   // 1
    FOUND_GAME_TEST,             // 2
    CREATED_GAME_TEST,           // 3
    GAMES_OCCUPIED_TEST,         // 4
    MENU_PREPERATION_FAIL_GI,    // 5
    MENU_PREPERATION_FAIL_PI,    // 6
    NO_CREATED_GAME_WAIT,        // 7
    SCND_PL_FOUND_WAIT,          // 8
    SCND_PL_NOT_FOUND_WAIT,      // 9
    NOT_IN_GAME_INIT,            // 10
    ALREADY_INDICATE,            // 11
    INDICATED_PL,                // 12
    INDICATED_QT,                // 13
    OTHER_NOT_INDICATE,          // 14
    OTHER_INDICATED_PL,          // 15
    OTHER_INDICATED_QT,          // 16
    MAX_MESSAGES_TEST            // 17
} message_server_id;

typedef enum{

NULL_POINTER,
STRUCT_FIRST,
NO_FIRST_NUMBER,
BUFF_OVF,
MESS_FIRST_VALUE
}error_id;

extern const char* test_message_server[];
int handler_error(char* buffer_error);
int test_menu_information_in_game(char* buffer_message, char* buffer_error, int file_descriptor);
int test_menu_information_not_in_game(char* buffer_message, char* buffer_error, int file_descriptor);
int test_quit_client(char* buffer_message, char* buffer_error, int file_descriptor);
int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor);
int test_setup_connection(char* buffer_error, int server_port);
int test_create_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_enter_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor);
int test_wait_second_player_init(char*buffer_message,char* buffer_error,int client_file_descriptor);
int test_wait_other_indicate(char* buffer_message, char* buffer_error, int file_descriptor);
int test_indicate_play(char* buffer_message, char* buffer_error, int client_file_descriptor);
#endif // TEST_SERVER_INTERFACE_H
