#include "client_main.h"
#include "client_send_data.h"

typedef enum{

GAME_FOUND = 1,

WAITING_FOR_GAME = 2,

GAME_NOT_FOUND = 3

}SECOND_LAYER_PLAY;

typedef enum{

FINDING_GAME = 1,

QUIT = 2

ENTERING_GAME = 3

}FIRST_LAYER;


void setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port){

	*(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);

	server_address->sin_family = AF_INET;

	server_address->sin_port = htons(port);


	int result_translation = 0;

	int status = 0;


	result_translation =  inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

	status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));

}


void printMenuSC(){

	printf("---- CLIENT MENU ----\n");
	printf("1. PLAY GAME\n");
	printf("2. QUIT GAME\n");

}

int handleoption(int option, int file_descriptor){

	char buffer_send[BUFFER_SIZE] = {0};

	char buffer_receive[BUFFER_SIZE] = {0};

	memset(buffer_send, 0, sizeof(buffer_send));

	memset(buffer_receive,0,sizeof(buffer_send));

	char* temporary_buffer;

	int temporary_option = 0, counter = 0, return_state = 0;

	buffer_send[counter++] = option + '0';

	buffer_send[counter++] = '|';


	switch(option){

			//for now we are going to use no memory here
			return_state = 0;
			temporary_buffer = "protocol has not been defined yet";

			break;

		}

	for(int i = 0;temporary_buffer[i]!='\0';i++){

		buffer_send[counter++]=temporary_buffer[i];

	}


	int bytes_receive = send_data_to_server(buffer_receive, buffer_send , file_descriptor);

	return return_state;
}

int handleWaitGame(int file_descriptor){

int number_of_bytes = read_all(file_descriptor, buffer, length);

if(number_of_bytes<=0){

printf("Error, we have received 0 bytes: indicates that connection was lost\n");


return 1;

}else{

return 0;

}

}


void handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0};

	char buffer_receive[BUFFER_SIZE]={0};

	char* numbers_recevied;

	struct sockaddr_in server_address={0};

	int bytes_receive = 0, bytes_send = 0, option = 0, client_file_descriptor = 0, temporary_option = 0, first_number = -1, second_number = -1;

        bool quit = false;


	setupConnection(&client_file_descriptor, &server_address, server_port);

	while(!quit){

		printMenuSC();

		scanf("%d", &option);

		numbers_recevied = handleoption(option, client_file_descriptor);

		int counter = 0 ;

		first_number = numbers_recevied[counter++]- '0';
		second_number = numbers_recevied[++counter]- '0';

		temporary_option = first_number;
		switch(first_number){

			case FIRST_LAYER::FINDING_GAME:

				switch(second_number){

					case SECOND_LAYER::GAME_FOUND:

						//enterd a game, meanign we have found a match and the game will start in a few moments
						break;

					case SECOND_LAYER::WAITING_FOR_GAME:

						//game created, waiting for someone to enter the game, so we are going to call the read function, and when the sergver sends somethj9ing back, that means that someone has enterd, or timeout, meaning no one wanted to etner the game so the player has to quit

						break;


					case SECOND_LAYER::GAME_NOT_FOUND:

						//we are not able to find a game, the player is going to have to quit and try later


						temporary_option = FIRST_LAYER::QUIT;
						break;


				}

			case FIRST_LAYER::QUIT:

				quit = true;

				//this is just for quitting


			case FIRST_LAYER::ENTERING_GAME:


				//this is when we enter the game

		}


	}

}

int main(){

	printf("This is the client interface\n");

	int port = 8080;

	handleServerCommunication(port);
}
