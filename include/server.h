#pragma once

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
	ENTERING_CREATING_GAME = 0,
	QUIT = 1,
	RANDOM_MESSAGE = 2,
	MENU_PREPERATION = 3,
	WAITING_INIT = 4,
	KEEP_WAITING =5,
	PLAY_TIME = 6
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

RANDOM_MESSAGE = 3,

QUIT_CLIENT = 4, //there are several types of quit statments depending on the sitautoin 

MENU_PREPERATION = 5,

//the following enums are used when the game is created, and after both players receive the menu info, they need to click play/confirm that they are going to play

PL_QUIT = 6,

OT_QUIT = 7,

BT_READY = 8


}ProtocolmessageID;


//the followig numbers are going to have to be revised
const char* protocol_string_holder[] ={
"0|1|you have found a game",
"0|2|you have created a game",
"0|3|all games occupied",
"2|1|this is response to random message",
"1|1|server received quit statement, goodbye",
"3|7|%d|%d",
"1|8|player is quitting",
"1|4|other player is quitting",
"3|3|other player ready"

}


typedef enum{

SYS_POLL = 0,

TIME_EXPIRED_POLL = 1,

SYS_RECV = 2,

SYS_SEND = 3,

INIT_SEND_LEN = 4,

MESS_SEND_LEN = 5,

INIT_RECV_LEN = 6,

MESS_RECV_LEN_OVF = 7

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

PROT_SECOND_VALUE = 18

}}ErrormessageID;
const char* error_string_holder[]={
"Error in event driven poll syscall (errno str):%s",
"Error, time out expired for poll event",
"Error, recv syscall went wrong (errno str):%s"
"Error, send syscall went wrong (errno str): %s",
"Error, amount of bytes sent in init message is not equal to 4 bytes",
"Error, amount of bytes sent of real message is not equal to length/bytes of message"
"Error, amount of bytes received in init message is not equal to 4 bytes",
"Error, amount of bytes recevied in init message is overflowing (>BUFFER_SIZE)"
"Error, amount of bytes received in real mesage is not equal to length/bytes of init message"
"Error, message length is too large: overflow",
"Error, message odes not have the correct structure",
"Error, first protocol number was not found",
"Error, second protocol number was not found"
"Error, first seperator was not found in the following message: %s",
"Error, second sepeartor was not found in the folloiwn gmessage: %s",
"Error, null message",
"Error, first protocol number is out of range (value not correct)",
"Error, second protocol number is out of range (value not correct)",
}

extern pthread_mutex_t mutex_game_list;
extern pthread_mutex_t mutex_thread_counter;
extern pthread_cond_t conditional_variable;
extern int counter_thread;

void* handle_client(void* arg);
void initilizeGames(game_struct_players* game_list);
