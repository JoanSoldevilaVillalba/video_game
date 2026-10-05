#include "test_server_main.h"
#include "test_server_interface.h"
#include "test_server_logic.h"

int main(){

	int port = 8080;

	char buffer_send[BUFFER_SIZE];

	char buffer_receive[BUFFER_SIZE];

	char buffer_error[BUFFER_SIZE];


	int client_file_descriptor = test_setup_connection(buffer_error, port);

	if(client_file_descriptor <0){

		return client_file_descriptor;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "2|random message init"); //this is the random message that we are going to have to send to the server side


	int message_send_result = test_send_message(buffer_send, buffer_error, client_file_descriptor);

	if(message_send_result <0){

		close(client_file_descriptor);

		return message_send_result;

	}

	int message_receive_result =  test_receive_message(buffer_receive,buffer_error, client_file_descriptor);

	if(message_receive_result <0){

		close(client_file_descriptor);

		return message_receive_result;

	}

	//this client specificly is going to find a game rather than create a game

	//we are first going to make it sleep for a second in order for the other client to have time for craeting the game that this client is going to try to find and enter

	sleep(2);

	snprintf(buffer_send, BUFFER_SIZE, "%s","0|find game for client");

	int result_find_game = test_enter_game_client(buffer_send,  buffer_error, client_file_descriptor);

	if(result_find_game == -1){

		close(client_file_descriptor);

		return result_find_game;

	}

	//after finding game, we are going to have to ask for menu information

	snprintf(buffer_send, BUFFER_SIZE, "%s", "3|ask menu info");

	int result_menu_information = test_menu_information_in_game(buffer_send, buffer_error, client_file_descriptor);

	if(result_menu_information == -1){

		close(client_file_descriptor);

		return result_menu_information;

	}

	int indication = 1;

	snprintf(buffer_send, BUFFER_SIZE, "4|%d",indication);

	sleep(2); 

	int result_indicate_play = test_indicate_play(buffer_send,buffer_error, client_file_descriptor);

	if(result_indicate_play == -1){

		close(client_file_descriptor);

		return result_menu_information;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "7|client wants quit");

	int result_quit_client = test_quit_client(buffer_send, buffer_error, client_file_descriptor);

	if(result_quit_client==-1){

		close(client_file_descriptor);

		return result_menu_information;

	}


	close(client_file_descriptor);

	return 0;

}
