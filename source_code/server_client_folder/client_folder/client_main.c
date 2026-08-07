#include "client_main.h"
#include "client_send_data.h"

typedef enum{

GAME_FOUND = 1,

WAITING_FOR_GAME = 2,

GAME_NOT_FOUND = 3

}SECOND_LAYER_PLAY;

typedef enum{

ENTERING_CREATING_GAME = 0,

QUIT = 1,

RANDOM_MESSAGE = 2

}FIRST_LAYER;


int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port){

	*(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);

	if(*(client_file_descriptor)<0){

		printf("Error, setupConnection: %s\n",strerror( errno));

		return -1;

	}


	server_address->sin_family = AF_INET;

	server_address->sin_port = htons(port);


	int result_translation = 0;

	int status = 0;


	result_translation =  inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

	if(result_translation <=0){

		printf("Error, setupConnection, inet_pton(possible incorrect format): %c\n", strerror(errno));

		return -1;

	}


	if(result_translation<0){

		printf("Error, setupConnection: %s\n",strerror(errno));

		return -1;

	}

	status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));

	if(status<0){

		printf("Error, setupConnection: %s\n",strerror(errno));

		return -1;

	}


	return 0;

}


void printMenuSC(){

	printf("---- CLIENT MENU ----\n");
	printf("1. PLAY GAME\n");
	printf("2. QUIT GAME\n");

}

int handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0}, buffer_receive[BUFFER_SIZE]={0}; const char* temporary_buffer =NULL;

	struct sockaddr_in server_address;

	ssize_t bytes_receive = 0, bytes_send = 0;

	int option = 0, client_file_descriptor = 0, first_number = -1, second_number = -1, temporal_length_recevied = 0;

        bool quit = false, communicate = false;



	memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive));

	memset(&server_address,0,sizeof(server_address));

	if(setupConnection(&client_file_descriptor, &server_address, server_port) == -1){

		printf("Error, handleServerCommmunication: unable to establish connection with the server\nExiting...");

		return -1;

	}

	printf("\n----- Client Interface -----\n");

	while(!quit){

		temporary_buffer = NULL;

		memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive));

		if(communicate == false){

			printMenuSC();

			scanf("%d", &option);

			switch(option){

				case ENTERING_CREATING_GAME:

					/*
typedef enum{

GAME_FOUND = 1,

WAITING_FOR_GAME = 2,

GAME_NOT_FOUND = 3

}SECOND_LAYER_PLAY;

typedef enum{

ENTERING_CREATING_GAME = 0,

QUIT = 1

}FIRST_LAYER;
				*/

					temporary_buffer = "0|0|client wants game";

					printf("client sending the following message: %s\n", temporary_buffer);

					bytes_send = send_framed_message(client_file_descriptor, buffer_send, (uint32_t)strlen(temporary_buffer));

					bytes_receive = receive_framed_message(client_file_descriptor, buffer_receive, (ssize_t)BUFFER_SIZE);

					first_number = option; //first option is ENTERING_CREATING_GAME

					counter = 0;

					counter = counter + 3;

					second_number = buffer_receive[counter] - '0'; //conveting char to int for second_number

					communicate = true;

					break;

				case QUIT:

					temporary_buffer ="1|0|client wants to quit";

					printf("client sending the following message: %s\n", temporary_buffer);

					strncpy(buffer_send, temporary_buffer, strlen(temporary_buffer));

					buffer_send[strlen(temporary_buffer)] = '\0';

					bytes_send = send_framed_message(client_file_descriptor, buffer_send, (uint32_t)strlen(temporary_buffer));

					bytes_receive = receive_framed_message(client_file_descriptor, buffer_receive, (ssize_t)BUFFER_SIZE);

					printf("server has responded with the following message: %s\n", buffer_receive);

					communicate = false;

					quit = true;

					first_number = 1;

					continue;

					break;

				case RANDOM_MESSAGE:

					temporary_buffer = "2|0|This is a random message";

					strncpy(buffer_send, temporary_buffer,strlen(temporary_buffer));

					bytes_send = send_framed_message(client_file_descriptor, buffer_send, (uint32_t)strlen(temporary_buffer));

					bytes_receive = receive_framed_message(client_file_descriptor, buffer_receive, (ssize_t)BUFFER_SIZE);

					printf("The server has responded with the following: %s\n", buffer_receive);

					continue;//continue , becauyse this message does not really follow the main protocol. THis is just an extra feature that we  wiill probably use for debuging or for other things not yet implemeented

					break;


			}



		}


		memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive));

		temporary_buffer = NULL;

		switch(first_number){


			case ENTERING_CREATING_GAME:

				switch(second_number){

					case WAITING_FOR_GAME:


						printf("We are waiting for a second player to enter our game\n");

						printf("Waiting for servers response ...\n");

						bytes_receive = receive_framed_message(client_file_descriptor, buffer_receive, (ssize_t)BUFFER_SIZEE);

						communicate = false;

						first_number = 0;

						counter = counter + 3; //we are accessing the second numnber of the protocol message that the server has sent us

						second_number = buffer_receive[counter];

						break;

					case GAME_NOT_FOUND :

						printf("The server was not able to find a game for us\n");

						first_number = QUIT;

						communicate = true; //no need for  memory.

						break;

					case GAME_FOUND:


						break;




				}







		}

		strncpy(buffer_send, temporary_buffer, strlen(temporary_buffer));

		buffer_send[strlen(temporary_buffer)] = '\0';

		bytes_send = send_framed_message(client_file_descriptor, buffer_send, (uint32_t)strlen(temporary_buffer));

		if(bytes_send == -1){

			printf("Error, handleServerCommmunication: unable to send the following message to the server:%s\n", temporary_buffer);

			close(client_file_descriptor);

			return -1;

		}

		bytes_receive = receive_framed_message(client_file_descriptor, buffer_receive, BUFFER_SIZE);

		if(bytes_receive == -1){

			printf("Error, handleServerCommmunication: closing connection now\n");

			close(client_file_descriptor);

			return -1;

		}

		sleep(5);

	}

	close(client_file_descriptor);


	return 0;

}

int main(){

	int port = 8080;

	int result_communication = handleServerCommunication(port);

	if(result_communication == -1){

		printf("Error, closing client now\n");

	}else{

		printf("Closing client with no problems\n");

	}

}
