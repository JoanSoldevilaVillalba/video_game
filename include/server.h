#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 64

#define MAX_GAMES_SIZE 2

#define MAX_CLIENT_THREADS (MAX_GAMES_SIZE*2)


typedef enum{

	ENTERING_CREATING_GAME = 0,

	QUIT = 1,

	RANDOM_MESSAGE = 2,

	MENU_PREPERATION = 3

}FIRST_LAYER;


typedef struct{

int first_player;

int second_player;

char name[62];

int game_id;

pthread_cond_t game_condition;

}game_struct_players;

typedef struct{

	int socket_fd;

	game_struct_players* pointer_list_game;

}struct_client;


pthread_mutex_t mutex_game_list;

pthread_mutex_t mutex_thread_counter;

pthread_cond_t conditional_variable;

int counter_thread;

#include "server_send_data.h"

#include "server_logic.h"


void* handle_client(void* arg);

void initilizeGames(game_struct_players* game_list);
