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

void handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0}, buffer_receive[BUFFER_SIZE]={0}; char* temporary_buffer =NULL;

	struct sockaddr_in server_address={0};

	int bytes_receive = 0, bytes_send = 0, option = 0, client_file_descriptor = 0, first_number = -1, second_number = -1, temporal_length_recevied = 0;

        bool quit = false, communicate = true;



	memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_send));

	setupConnection(&client_file_descriptor, &server_address, server_port);

	while(!quit){


		if (communicate){

			printMenuSC();

			scanf("%d", &option);

			switch(option){

				case 1:
					temporary_buffer="1|0|user wants to create/find game";
					break;

				case 2:
					temporary_buffer="2|0|user wants to quit game";

					break;
				default:

					printf("Error, invalid number\n");

					continue;


		}

		strcpy(buffer_send, temporary_buffer);

		temporal_length_recevied = send_data_to_server(buffer_receive,buffer_send , client_file_descriptor);

		}

		int counter  = 0;

		first_number = buffer_receive[counter++] - '0';
		counter++;
		second_number = buffer_receive[counter++] - '0';
		++counter;

		switch(first_number){

			case FINDING_GAME:

				switch(second_number){

					case GAME_FOUND:

						first_number = ENTERING_GAME;

						communicate = false;

						break;

					case WAITING_FOR_GAME:

						char wait_buffer[BUFFER_SIZE] = {0};

					        memset(buffer_send, 0, sizeof(buffer_receive));

						struct timeval timeout;

						timeout.tv_sec = 10;

						timeout.tv_usec = 0;

						setsockopt(client_file_descriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

						int length_recevied = read_all(client_file_descriptor, wait_buffer, BUFFER_SIZE-1);

						timeout.tv_sec = 0;
                                                timeout.tv_usec = 0;

                                                setsockopt(client_file_descriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

						if(length_recevied<0){

							if(errno == EWOULDBLOCK || errno == EAGAIN){


							printf("Error, there was a timeout\n");

							}

							first_number = FINDING_GAME;

							second_number = GAME_NOT_FOUND;

						}else{

							communicate = false;

							//in case the server was able to find a game for us, now we can switch context
							first_number = FINDING_GAME; //we are switching context to ENTERING_GAME, before switching to ENTERING_GAME; we are firstr going into 

							second_number = GAME_FOUND;

							}

						break;


					case GAME_NOT_FOUND:

						printf("A game was not found\n");

						first_number = QUIT;

						communicate = false;

						break;


				}

			break;

			case QUIT:

				printf("Quitting game....\n");

				quit = true;

				break;

			case ENTERING_GAME:


				//we have not yet done this,we are going to have to do this later probably

				break;



		}


	}

}

int main(){

	printf("This is the client interface\n");

	int port = 8080;

	handleServerCommunication(port);
}
