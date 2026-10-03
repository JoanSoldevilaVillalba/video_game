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
#define TM_EXP_WAIT_GAME 2
#define TM_EXP_POLL 10000

typedef enum {
	ENTERING_CREATING_GAME_STATE,
	RANDOM_MESSAGE_STATE,
	WAIT_GAME_CREATING_STATE,
	MENU_PREPERATION_STATE,
	INIT_GAME_STATE,
	WAIT_SECOND_PLAYER_READY_STATE,
	PLAY_TIME_STATE,
	QUIT_CLIENT_STATE,
	QUIT_SERVER_STATE
} FIRST_LAYER;

typedef struct {
    int player_id[2];
    char name[62];
    int game_id;
    pthread_cond_t game_condition;
    pthread_mutex_t mutex_game_list;
    bool ready_player[2];
} game_struct_players;

typedef struct {
	int socket_fd;
	game_struct_players* pointer_list_game;
} struct_client;

typedef enum{
	FOUND_GAME__ENTER_STATE ,
	CREATE_GAME__ENTER_STATE,
	GAMES_FULL__ENTER_STATE ,
	IN_GAME__ENTER_STATE ,
	RESPONSE_MESSAGE__RANDOM_STATE,
	NO_SCND_PL__WAIT_CREATE_STATE,
	SCND_PL_FOUND__WAIT_CREATE_STATE,
	NO_CREATED_GAME__WAIT_CREATE_STATE,
	NOT_IN_GAME__MENU_PREP_STATE,
	INDEX_PL_ERR__MENU_PREP_STATE,
	INDEX_GM_ERR__MENU_PREP_STATE,
	MENU_INFO__MENU_PREP_STATE,
	NOT_IN_GAME__INIT_STATE,
	ALREADY_INDICATED_GAME__INIT_STATE,
	INDICATED_PL_GAME__INIT_STATE,
	INDICATED_QUIT_GAME__INIT_STATE,
	NOT_IN_GAME__W_SCND_PL_STATE,
	NOT_INDICATED__W_SCND_PL_STATE,
	OTHER_PL_QUIT__W_SCND_PL_STATE,
	OTHER_PL_NOT_IND__W_SCND_PL_STATE,
	OTHER_PL_PLAY_IND__W_SCND_PL_STATE,
	QUIT_STATMENT__QC_STATE,
	QUIT_STATMENT__QS_STATE,
	DEFAULT
} ProtocolmessageID;
typedef enum{
NULL_POINTER,
STRUCT_FIRST,
NO_FIRST_NUMBER,
BUFF_OVF,
MESS_FIRST_VALUE
}errorStringId;

extern const char* protocol_string_holder[];
extern const char* error_string_holder[];
extern pthread_mutex_t mutex_thread_counter;
extern int counter_thread;

void* handle_client(void* arg);
void initilizeGames(game_struct_players* game_list);

#endif //SERVER_H
