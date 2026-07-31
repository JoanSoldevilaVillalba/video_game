#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "client_send_data.h"

#define BUFFER_SIZE 64


void setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port);

void printMenuSC();

int handleoption(int option, int file_descriptor);

void handleServerCommunication(int server_port);
