
#include "test_server_interface.h"
#include "test_server_logic.h"

const char* test_message_server[] = {
    [RANDOM_MESSAGE_TEST] = "2|1|this is response to random message",
    [QUIT_CLIENT_MESSAGE_TEST] = "1|1|server received quit statement, goodbye",
    [FOUND_GAME_TEST] = "0|1|you have found a game",
    [CREATED_GAME_TEST] = "0|2|you have created a game",
    [GAMES_OCCUPIED_TEST] = "0|3|all games occupied",
    [GAME_EXPERATION_TEST] = "0|4|Game not found: time expired",
    [GAME_NO_SCND_PLAYER_TEST] = "0|5|Second player for your match is not found yet",
    [MENU_PREPERATION_INFO] = "3|7|",
    [MENU_PREPERATION_FAIL_GI] = "3|4|Error, game index",
    [MENU_PREPERATION_FAIL_PI]="3|5|Error, player index",
    [MAX_MESSAGES_TEST] = NULL
};

int handler_error(char* buffer_error){

	printf("An error has occurred: %s\n", buffer_error);

	printf("Testing is quitting now, goodbye\n");

	return -1;
}

bool compare_strings(char* buffer_message, const char* compare_messages[], char* buffer_error, int dimensions){

	printf("Server responded with the following message: %s\n", buffer_message);
	printf("Expected messages are the following: \n");

	for(int i = 0;i<dimensions;i++){

		printf("possible message number %d: %s\n",i, compare_messages[i]);

		if(strcmp(buffer_message, compare_messages[i])==0){

			return true;

		}

	}

	snprintf(buffer_error, BUFFER_SIZE, "%s", "Error, message recevied from server not expected");
	return false;

}

int test_init_wait_client(char* buffer_message, char* buffer_error, int file_descriptor){

	printf("\n --- Testing init waiting mechanics -----\n");

	ssize_t result = send_validated_message(buffer_message, buffer_error, file_descriptor);

	if(result == -1){

		return handler_error(buffer_error);

	}

	result = receive_validated_message(buffer_message, buffer_error, file_descriptor);

	if(result == -1){

		return handler_error(buffer_error);

	}

	printf("Server has sent the following message: %s\n", buffer_message);

	printf("Test was completed correctly\n");

}


int test_menu_information_in_game(char* buffer_message, char* buffer_error, int file_descriptor){
//we need to keep in mind the following: this is only called when a game is created, meaning the server has already registesrt the playuer inside of a game slot

	printf("\n ---- testing menu information when client is in a game ----- \n");

	ssize_t result = send_validated_message(buffer_message, buffer_error, file_descriptor);

	if(result == -1){

		return handler_error(buffer_error);

	}

	result = receive_validated_message(buffer_message, buffer_error, file_descriptor);

	if(result == -1){

		return handler_error(buffer_error);

	}

	const char* compare_buffer[1] = {test_message_server[MENU_PREPERATION_INFO]};

	bool equal = compare_strings(buffer_message, compare_buffer, buffer_error, 1); //last parameter indicates the amount of strings that the server can resonid with that are valid

	if(!equal){

		return handler_error(buffer_error);

	}

	printf("Test was completed correctly\n");

	return 0;

}


int test_menu_information_not_in_game(char* buffer_message, char* buffer_error, int file_descriptor){

	printf("\n ---- testing menu information when client is not in a game ----- \n");

	ssize_t result_temporary = send_validated_message(buffer_message, buffer_error, file_descriptor);

	if(result_temporary==-1){

		return handler_error(buffer_error);

	}

	result_temporary = receive_validated_message(buffer_message, buffer_error, file_descriptor);

	if(result_temporary==-1){

		return handler_error(buffer_error);

	}

	const char* compare_buffer[2] = {test_message_server[MENU_PREPERATION_FAIL_GI], test_message_server[MENU_PREPERATION_FAIL_PI]};

	bool equal = compare_strings(buffer_message, compare_buffer, buffer_error, 2);

	if(!equal){

		return handler_error(buffer_error);

	}

	printf("Test was completed correctly\n");

	return 0;

}

int test_quit_client(char* buffer_message, char* buffer_error, int file_descriptor) {
    printf("\n ----- testing quit statement from client ------ \n");
    int result = send_validated_message(buffer_message, buffer_error, file_descriptor);
    if (result == -1) {

	return handler_error(buffer_error);

    }

    result = receive_validated_message(buffer_message, buffer_error, file_descriptor);

    if (result == -1) {

	return handler_error(buffer_error);

    }


    const char* compare_buffer[1] = {test_message_server[QUIT_CLIENT_MESSAGE_TEST]};

    bool equal = compare_strings(buffer_message, compare_buffer,buffer_error, 1);

    if(!equal){

	return handler_error(buffer_error);

    }

    printf("Test was completed correctly\n");
    return 0;
}

int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor) {
    printf("\n------- testing receive message ------- \n");

    int result = receive_validated_message(buffer_message, buffer_error, file_descriptor);

    if(result == -1){

	return handler_error(buffer_error);

    }

    const char* compare_buffer[1] = {test_message_server[RANDOM_MESSAGE_TEST]};

    bool equal = compare_strings(buffer_message, compare_buffer, buffer_error, 1);

    if(!equal){

	return handler_error(buffer_error);

    }

    printf("Test was completed correctly\n");

    return 0;
}

int test_setup_connection(char* buffer_error, int server_port) {
    struct sockaddr_in server_address;
    int file_descriptor = -1;

    printf("----- Testing Connection Setup -----\n");
    int result = setupConnection(&file_descriptor, &server_address, server_port, buffer_error);
    if (result  == -1) {

	return handler_error(buffer_error);

    }

    printf("Test was completed correctly\n");

    return file_descriptor;
}

int test_enter_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor){
    printf("\n ----- testing enter game action ----- \n");
    int result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);
    if (result == -1) {

	return handler_error(buffer_error);

    }

    result = receive_validated_message(buffer_message, buffer_error, client_file_descriptor);
    if (result == -1) {

	return handler_error(buffer_error);

    }

    const char* compare_buffer[1]={test_message_server[FOUND_GAME_TEST]};

    bool equal = compare_strings(buffer_message,compare_buffer, buffer_error, 1);

    if(!equal){

	return handler_error(buffer_error);

    }

    printf("Test was completed correctly\n");

    return 0;
}

int test_create_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor) {
    printf("\n ----- testing create game action ----- \n");

    int result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);

    if (result == -1) {

	return handler_error(buffer_error);

    }

    result = receive_validated_message(buffer_message, buffer_error, client_file_descriptor);

    if (result == -1) {

	return handler_error(buffer_message);

    }

    const char* compare_buffer[1] = {test_message_server[CREATED_GAME_TEST]};
    bool equal = compare_strings(buffer_message,compare_buffer, buffer_error, 1);
    if(!equal){

	return handler_error(buffer_error);

    }
    //after creating a game, the server is now going to wait until someone else enters the game, we are going to have to wait
    result = receive_validated_message(buffer_message, buffer_error, client_file_descriptor);

    if(result == -1){

        return handler_error(buffer_error);

    }

    compare_buffer[0] = test_message_server[GAME_EXPERATION_TEST];
    equal = compare_strings(buffer_message,compare_buffer,buffer_error, 1);
    if(!equal){

	return handler_error(buffer_error);

    }

    printf("Test was completed correctly\n");

    return 0;

}

int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor) {
    printf("\n------ testing message sending ------\n");
    ssize_t result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);
    if (result == -1) {

      return handler_error(buffer_error);

    }


    printf("Test was completed correctly\n");

    return 0;
}
