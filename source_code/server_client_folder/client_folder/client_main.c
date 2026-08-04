#include "client_main.h"
#include "client_send_data.h"

typedef enum{

GAME_FOUND = 1,

WAITING_FOR_GAME = 2,

GAME_NOT_FOUND = 3

}SECOND_LAYER_PLAY;

typedef enum{

FINDING_GAME = 1,

QUIT = 2,

ENTERING_GAME = 3

}FIRST_LAYER;


int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port){

	*(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);

	if(*(client_file_descriptor)<0){

		printf("Error: %s\n",strerror( errno));

		return -1;

	}


	server_address->sin_family = AF_INET;

	server_address->sin_port = htons(port);


	int result_translation = 0;

	int status = 0;


	result_translation =  inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

	if(result_translation<0){

		printf("Error:",strerror(errno));

		return -1;

	}

	status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));

	if(status<0){

		printf("Error: %s\n",strerror(errno));

		return -1;

	}

}


void printMenuSC(){

	printf("---- CLIENT MENU ----\n");
	printf("1. PLAY GAME\n");
	printf("2. QUIT GAME\n");

}

void handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0}, buffer_receive[BUFFER_SIZE]={0}; char* temporary_buffer =NULL;

	struct sockaddr_in server_address;

	int bytes_receive = 0, bytes_send = 0, option = 0, client_file_descriptor = 0, first_number = -1, second_number = -1, temporal_length_recevied = 0;

        bool quit = false, communicate = true;



	memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive));

	memset(server_address,0,sizeof(server_address));

	if(setupConnection(&client_file_descriptor, &server_address, server_port) == -1){

		printf("Error: unable to establish connection with the server\nExiting...");

		return -1;

	}

	while(!quit){

		temporary_buffer = "0|0|testisng communication";

		//rememebr that because temporary_buffer is a const char*, the compiler is oging to automatilclyu going to add the null terminator at the end of the string

		//we just want to send strlen(temprorary_buffer) length, not all of the 64 bytes, the actual size of the buffer

		strncpy(buffer_send, temporary_buffer, strlen(temporary_buffer)-1);

		buffer_send[temporary_buffer] = '\0';

		ssize_t data_sent = send_framed_message(client_file_descriptor, buffer_send, (uint32_t)(sizeof(temporary)-1));

		if(data_sent == -1){

			printf("Error, unable to send the following message to the server:%s\n", temporary_buffer);

			close(client_file_descriptor);

			return -1;

		}

		printf("Client has succesfuly sent the following message: %s\n", temporary_buffer);

		//now we are going to read what the server sends us back

		

	}

}

int main(){

	printf("This is the client interface\n");

	int port = 8080;

	int result_communication = handleServerCommunication(port);

	if(result_communication == -1){

		printf("Error, closing client now\n");

	}else{

		printf("Closing client with no problems\n");

	}

}
