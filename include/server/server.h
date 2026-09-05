#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#define BUFFER_SIZE 64
#define MAX_GAMES_SIZE 2
#define MAX_CLIENT_THREADS (MAX_GAMES_SIZE*2)

typedef enum {
	ENTERING_CREATING_GAME_STATE = 0,
	QUIT_CLIENT_STATE = 1,
	RANDOM_MESSAGE_STATE = 2,
	MENU_PREPERATION_STATE = 3,
	WAITING_INIT_STATE = 4,
	KEEP_WAITING_STATE =5,
	PLAY_TIME_STATE = 6
	QUIT_SERVER_STATE = 7
} FIRST_LAYER;

typedef struct {
    int player_id[2];
    char name[62];
    int game_id;
    pthread_cond_t game_condition;
    bool ready_player[2];
} game_struct_players;

typedef struct {
	int socket_fd;
	game_struct_players* pointer_list_game;
} struct_client;


typedef enum{
//error messages

//protocol messages (for client)

FOUND_GAME = 0,

CREATED_GAME = 1,

GAMES_OCCUPIED = 2,

RANDOM_MESSAGE_PROT = 3,

QUIT_CLIENT = 4, //there are several types of quit statments depending on the sitautoin 

MENU_PREPERATION_PROT = 5,

//the following enums are used when the game is created, and after both players receive the menu info, they need to click play/confirm that they are going to play

PL_QUIT = 6,

OT_QUIT = 7,

BT_READY = 8,

GAME_EXPERATION = 9,

INVALID_OPT = 10,

QUIT_SERVER = 11
}ProtocolmessageID;

typedef enum{

SYS_POLL = 0,

TIME_EXPIRED_POLL = 1,

SYS_RECV = 2,

SYS_SEND = 3,

INIT_SEND_LEN = 4,

MESS_SEND_LEN = 5,

INIT_RECV_LEN = 6,

MESS_RECV_LEN_OVF = 7,

MESS_RECV_LEN = 8,

//the folloinwg are errors that do not deal with code that uses POSIX syscalls

BUFF_OVF = 9,

BUFF_STRUCT = 10,

BUFF_PROT_FIRST = 11,

BUFF_PROT_SECOND = 12,

MENU_INDEX = 13,

STRUCT_FIRST = 14,

STRUCT_SECOND = 15,

NULL_MESS = 16,

PROT_FIRST_VALUE = 17,

PROT_SECOND_VALUE = 18,

NO_FIRST_NUMBER = 19,

NO_SECOND_NUMBER = 20,

}ErrormessageID;


//the followig numbers are going to have to be revised
extern const char* protocol_string_holder[];

extern const char* error_string_holder[];

extern pthread_mutex_t mutex_game_list;
extern pthread_mutex_t mutex_thread_counter;
extern pthread_cond_t conditional_variable;
extern int counter_thread;

void* handle_client(void* arg);
void initilizeGames(game_struct_players* game_list);


#endif //SERVER_H
