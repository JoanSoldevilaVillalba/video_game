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

		printf("Error: %s\n",strerror(errno));

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

int handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0}, buffer_receive[BUFFER_SIZE]={0}; char* temporary_buffer =NULL;

	struct sockaddr_in server_address;

	ssize_t bytes_receive = 0, bytes_send = 0; 

	int option = 0, client_file_descriptor = 0, first_number = -1, second_number = -1, temporal_length_recevied = 0;

        bool quit = false, communicate = true;



	memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive));

	memset(&server_address,0,sizeof(server_address));

	if(setupConnection(&client_file_descriptor, &server_address, server_port) == -1){

		printf("Error: unable to establish connection with the server\nExiting...");

		return -1;

	}

	while(!quit){

		temporary_buffer = NULL;

		memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive));

		temporary_buffer = "0|0|testing communication";

		strncpy(buffer_send, temporary_buffer, strlen(temporary_buffer));

		buffer_send[strlen(temporary_buffer)] = '\0';

		bytes_send = send_framed_message(client_file_descriptor, buffer_send, (uint32_t)strlen(temporary_buffer));

		if(bytes_send == -1){

			printf("Error, unable to send the following message to the server:%s\n", temporary_buffer);

			close(client_file_descriptor);

			return -1;

		}

		printf("Client has succesfuly sent the following message: %s\n", temporary_buffer);


		bytes_receive = receive_framed_message(client_file_descriptor, buffer_receive, BUFFER_SIZE - 1);

		if(bytes_receive == -1){

			printf("Error, closing connection now\n");

			close(client_file_descriptor);

			return -1;

		}


		printf("The server has responded with the following message %s\n\n\n", buffer_receive);

		sleep(5);

	}


	return 0;

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
