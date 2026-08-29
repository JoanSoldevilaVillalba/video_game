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

ENTERD_GAME = 0,

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

typedef enum{



}ErrormessageID;

const char* protocol_string_holder[] ={
"you have entered a game",
"you have created a game",
"all games occupied",
"this is response to random message",
"server received quit statement, goodbye",
"",
"player is quitting",
"other player is quitting",
"other player ready"

}


extern pthread_mutex_t mutex_game_list;
extern pthread_mutex_t mutex_thread_counter;
extern pthread_cond_t conditional_variable;
extern int counter_thread;

void* handle_client(void* arg);
void initilizeGames(game_struct_players* game_list);
