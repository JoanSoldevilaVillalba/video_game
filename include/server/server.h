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
#define TM_EXP_WAIT_GAME 2 // the player is going to wait for 60 seconds every time that she or he desires to wait for someone to enter the game
#define TM_EXP_POLL 10000 //this time experation limit is used for the low level impellemtnation of sending and receiving information from the client, therefor>


typedef enum {
	ENTERING_CREATING_GAME_STATE = 0,
	RANDOM_MESSAGE_STATE = 1,
	WAIT_GAME_CREATING_STATE = 2,
	MENU_PREPERATION_STATE = 3,
	INIT_GAME_STATE = 4,
	WAIT_SECOND_PLAYER_READY_STATE = 5, //this state is used for players to wait for the second player that enterd the game
	PLAY_TIME_STATE = 6,
	QUIT_CLIENT_STATE = 7,
	QUIT_SERVER_STATE = 8

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

//each of these indices indicate different messages that each enum state of the server can send back to. There might be redundancy, repeated messages.
//this redundancy is added to keep messages in their former enum state that they belong to

FOUND_GAME__ENTER_STATE = 0,
CREATE_GAME__ENTER_STATE = 1,
GAMES_FULL__ENTER_STATE = 2,
IN_GAME__ENTER_STATE = 3,

RESPONSE_MESSAGE__RANDOM_STATE = 4,

NO_SCND_PL__WAIT_CREATE_STATE = 5,
SCND_PL_FOUND__WAIT_CREATE_STATE = 6,
NO_CREATED_GAME__WAIT_CREATE_STATE = 7,

NOT_IN_GAME__MENU_PREP_STATE = 8,
//the following cases happen when for some reason one of the indeces is set to -1 or some value that is not correct, we are going to have to indicate this to the client.
//this is added to protcol_message instead of error message because we are sending this to client
INDEX_PL_ERR__MENU_PREP_STATE = 9,
INDEX_GM_ERR__MENU_PREP_STATE = 10,
MENU_INFO__MENU_PREP_STATE = 11,

NOT_IN_GAME__INIT_STATE = 12,
ALREADY_INDICATED_GAME__INIT_STATE = 13,
INDICATED_PL_GAME__INIT_STATE = 14,
INDICATED_QUIT_GAME__INIT_STATE = 15,


NOT_IN_GAME__W_SCND_PL_STATE = 16,
NOT_INDICATED__W_SCND_PL_STATE = 17,
OTHER_PL_QUIT__W_SCND_PL_STATE = 18,
OTHER_PL_NOT_IND__W_SCND_PL_STATE = 19,
OTHER_PL_PLAY_IND__W_SCND_PL_STATE = 20,

QUIT_STATMENT__QC_STATE = 21,

QUIT_STATMENT__QS_STATE = 22,

DEFAULT = 23

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

NO_SECOND_NUMBER = 20


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
