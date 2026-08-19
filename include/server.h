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
    bool game_lock;
} game_struct_players;

typedef struct {
	int socket_fd;
	game_struct_players* pointer_list_game;
} struct_client;

extern pthread_mutex_t mutex_game_list;
extern pthread_mutex_t mutex_thread_counter;
extern pthread_cond_t conditional_variable;
extern int counter_thread;

void* handle_client(void* arg);
void initilizeGames(game_struct_players* game_list);
