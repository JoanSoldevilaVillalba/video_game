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
	MENU_PREPERATION = 3,
	WAITING_INIT = 4,
	KEEP_WAITING = 5,
	PLAY_TIME = 6
} FIRST_LAYER;


int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port);

void printMenuSC();

// the folowing function was eliminated: int handleoption(int option, int file_descriptor);

int handleServerCommunication(int server_port);
