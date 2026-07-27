//this file is going to contain the server code
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "server_send_data.h"

#define BUFFER_SIZE 64

#define MAX_GAMES_SIZE 2


typedef struct{

int first_player;

int second_player;

char name[62];

int game_id;
}game_struct_players;



/*
for creating a game, we need to keep in mind the following:
we should probably add more depth to the protocol to indicate what setp we are in for the specific type of message exchange we are doign with the client.

for creating/entering a game: 1

in this protocol:

1 game was creatd but no second player, user is idle watiing for someone to enter the game.

2 you have found an already created game

3 no games found and not able to createe game do to maximum server handle size.


*/
char* create_game(int temporary_fd, char* buffer_receive, int counter, game_struct_players* game_list){
	//when a game is inietilized for the first time, first_player attribute is givin to the player that creates the game.
	//after that when someone wants to create the game or enter a game, the attribute that the player will occupy will be second_player

	//when the lsit of games is initilized all attributyes are set to -1
	int i = 0;
	while(i<MAX_GAMES_SIZE){

		if((game_list+i)->game_id!=-1 && (game_list+i)->second_player==-1){

			(game_list+i)->second_player = temporary_fd;
			return "1|2|you have succesfuly entered a game";

		}else if((game_list+i)->game_id==-1){

			(game_list+i)->game_id = i;
			(game_list+i)->first_player=temporary_fd;
			//the game was succesfully added because game_list was passed as a pointer
			return "1|1|you have succesfuly created a game";

		}else{

			i++;
		}


	}

	//we need to keeep in mind that after creating a game/entering the game, maybe the player needs to wait for a while if the game is crated and there is no second player, we need a way to communicate this.

	return "1|3|All games are occupied, try again later";

}


void handle_client(int temporary_fd, game_struct_players* game_list){


	int client_game_id = -1;

	char buffer_receive[BUFFER_SIZE];

	char buffer_send [BUFFER_SIZE];

	int bytes_received = 0;

	int bytes_sent = 0;

	int result = 0;

	int counter = 0;

	bool quit = false;


	while(!quit){

		counter = 0;

		memset(buffer_receive,0,sizeof(buffer_receive));

		memset(buffer_send,0,sizeof(buffer_send));


		bytes_received = read_all(temporary_fd, buffer_receive, BUFFER_SIZE-1);

		if(bytes_received>0){

			buffer_receive[bytes_received] = '\0';

		}else{

			printf("Error, we recevied zero bytes\n Client probably disconnected, breaking\n");

			break;

		}

		printf("CLIENT NUMBER %d HAS SENT THE FOLLOWING MESSAGE: %s\n\n",temporary_fd, buffer_receive);

		result = buffer_receive[0] - '0';

		buffer_send[counter++] = result + '0';

		buffer_send[counter++] = '|';

		char* temporary_char_pointer;

		switch(result){


			case 1:

				//one indicates that the user wants to create/enter a game. Before creating a game, we are going to have to go through all game_list games, to see if a game has already been creatred and the created is waiting for someone to enter the game.
				temporary_char_pointer = create_game(temporary_fd, buffer_receive, counter, game_list);

				break;

			case 2:

				quit = true;

				break;

			default:

				temporary_char_pointer ="Error, invalid option was sent over";

				break;

		}

		for(int i =0;temporary_char_pointer[i]!='\0';i++){

			buffer_send[counter++] = temporary_char_pointer[i];

		}

		printf("the server is sending the following message\n");

		for(int i =0;buffer_send[i]!='\0';i++){

			printf("%c", buffer_send[i]);

		}

		printf("\n");

		bytes_sent = send_all(temporary_fd , buffer_send, BUFFER_SIZE);

		printf("bytes thave been sent: %d\n", bytes_sent);

	}

	close(temporary_fd);

}


void initilizeGames(game_struct_players* game_list){

	for(int i =0;i<MAX_GAMES_SIZE;i++){

		(game_list+i)->first_player = -1;

		(game_list+i)->second_player = -1;

		memset((game_list+i)->name,0,sizeof((game_list+i)->name));

		(game_list+i)->game_id = -1;

	}

}
int main()
{

	int server_file_descriptor = 0;

	int new_socket = 0;

	struct sockaddr_in address;

	int opt = 1;

	int port_number = 8080;


	//the server needs a way to be able to represent games, in this case we are using a strucc: both players(player are integers, indicating the file descriptro for the socket connection), game name aswell as game id (for now we ar ejust going to use game name)

	game_struct_players game_list[MAX_GAMES_SIZE];

	socklen_t addrlen = sizeof(address);

	server_file_descriptor = socket(AF_INET, SOCK_STREAM,0);


	initilizeGames(game_list);

	if(server_file_descriptor<0){

		printf("Error, the operating system was not able to assign us a file descriptor for the tcp ip socket that we wanted to create\n");

		return 1;
	}

	if(setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){

		printf("for some reason there was an error with configuring the socket\n");

		return 2;

	}


	address.sin_family = AF_INET;

	address.sin_addr.s_addr = INADDR_ANY;


	address.sin_port = htons(port_number);

	if (bind(server_file_descriptor, (struct sockaddr*)&address,sizeof(address))< 0) {

	        perror("bind failed");

	        exit(EXIT_FAILURE);
	}

	printf("THIS IS THE SERVER\n THE SERVER HAS THE FOLLOWING INFORMATION\n");

	printf("SERVER FAMILY: %hd\n", address.sin_family);

	printf("SERVER PORT NUMBER: %hu\n",ntohs(address.sin_port));

	printf("\nTHE SERVER IS READY\n\n\n");

	listen(server_file_descriptor,2);

	while(1){


		new_socket = accept(server_file_descriptor, (struct sockaddr*)&address,(socklen_t*)&addrlen);

		printf("\nSERVER CONNECTED TO CLIENT WITH FD: %d\n", new_socket);

		if(new_socket<0){

			printf("something went wrong, we were not able to create a socket or assigna file descriptor");

			exit(EXIT_FAILURE);

		}

		handle_client(new_socket, game_list);

		}

	close(server_file_descriptor);
}
