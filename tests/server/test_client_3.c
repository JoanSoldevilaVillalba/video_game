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

	snprintf(buffer_send, BUFFER_SIZE, "%s", "1|random message init"); //this is the random message that we are going to have to send to the server side


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

	snprintf(buffer_send, BUFFER_SIZE, "%s", "0|client wants game");

	int result_create_game =  test_create_game_client(buffer_send, buffer_error, client_file_descriptor);

	if(result_create_game == -1){

		close(client_file_descriptor);

		return result_create_game;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "2|Asking to wait");

	int result_wait_second_player = test_wait_second_player_init(buffer_send, buffer_error, client_file_descriptor);

	if(result_wait_second_player == -1){

		close(client_file_descriptor);

		return result_wait_second_player;

	}


	snprintf(buffer_send, BUFFER_SIZE, "%s", "3|ask menu info");

	int menu_information_result = test_menu_information_in_game(buffer_send, buffer_error, client_file_descriptor);

	if(menu_information_result == -1){

		close(client_file_descriptor);

		return menu_information_result;

	}

	int indication = 1;

	snprintf(buffer_send, BUFFER_SIZE, "4|%d", indication);

	int result_indicate_play = test_indicate_play(buffer_send, buffer_error, client_file_descriptor);

	if(result_indicate_play == -1){

		close(client_file_descriptor);

		return result_indicate_play;
	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "5|wait other indicate");

	int result_wait_other_indicate = test_wait_other_indicate(buffer_send, buffer_error, client_file_descriptor);

	if(result_wait_other_indicate == -1){

		close(client_file_descriptor);

		return result_wait_other_indicate;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "7|clients wants quit");

	int result_quit_init = test_quit_client(buffer_send, buffer_error, client_file_descriptor);

	if(result_quit_init == 1){

		close(client_file_descriptor);

		return result_quit_init;

	}

	close(client_file_descriptor);

	return 0;


}
