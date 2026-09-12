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

	snprintf(buffer_send, BUFFER_SIZE, "%s", "2|0|random message init"); //this is the random message that we are going to have to send to the server side


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

	snprintf(buffer_send, BUFFER_SIZE, "%s","3|0|client wants menu");
	int result_menu_fail = test_menu_information_not_in_game(buffer_send, buffer_error, client_file_descriptor);
	if(result_menu_fail <0){

		close(client_file_descriptor);

		return result_menu_fail;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "0|0|client wants game");

	int result_create_game = test_create_game_client(buffer_send, buffer_error, client_file_descriptor);

	if(result_create_game == -1){

		close(client_file_descriptor);

		return result_create_game;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "3|0|client wants menu");
	int result_menu_good = test_menu_information_in_game(buffer_send, buffer_error, client_file_descriptor);
	if(result_menu_good<0){

		close(client_file_descriptor);

		return result_menu_good;

	}

	snprintf(buffer_send, BUFFER_SIZE, "%s", "1|0|client wants to quit");

	int result_quit_client_statment =  test_quit_client(buffer_send, buffer_error, client_file_descriptor);

	if(result_quit_client_statment <0){

		close(client_file_descriptor);

		return result_quit_client_statment;

	}

	close(client_file_descriptor);

	printf("We have finished with the tests (all tests passed), goodbye....");

	return 0;

}
