#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "client_send_data.h"
#include <errno.h>
#include <sys/time.h>

#define BUFFER_SIZE 64

typedef enum {
    ENTERING_CREATING_GAME = 0,
    QUIT = 1,
    RANDOM_MESSAGE = 2,
    GAME_PLAY = 3
} CLIENT_FIRST_LAYER;

typedef enum {
    WAITING_FOR_GAME = 0,
    GAME_NOT_FOUND = 1,
    GAME_FOUND = 2,
    STILL_WAITING = 3
} CLIENT_ENTERING_GAME_STATES;

typedef enum {
    PROLOGUE = 0,
    MENU = 1,
    PLAY_TIME = 2
} CLIENT_GAME_PLAY_STATES;

typedef enum {
    CLIENT_QUIT = 0,
    SERVER_QUIT = 1,
    BROKEN_QUIT = 2,
    OTHER_PLAYER_QUIT = 3,
    YOU_PLAYER_QUIT = 4
} CLIENT_QUIT_STATES;

int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port);

void printMenuSC();

int handleServerCommunication(int server_port);
