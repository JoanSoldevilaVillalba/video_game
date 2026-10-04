
#include "test_server_interface.h"
#include "test_server_logic.h"

#include "server_logic.h"
/*
const char* protocol_string_holder[] ={
[FOUND_GAME__ENTER_STATE] = "0|client found game",
[CREATE_GAME__ENTER_STATE] = "0|client created game",
[GAMES_FULL__ENTER_STATE] = "0|all games occupied",
[IN_GAME__ENTER_STATE] = "0|client in game already",

[RESPONSE_MESSAGE__RANDOM_STATE] = "1|Server reserved message",

[NO_SCND_PL__WAIT_CREATE_STATE] = "2|no second player",
[SCND_PL_FOUND__WAIT_CREATE_STATE] = "2|second player found",
[NO_CREATED_GAME__WAIT_CREATE_STATE] = "2|Client not in game",

[NOT_IN_GAME__MENU_PREP_STATE] = "3|Client not in game",
[INDEX_PL_ERR__MENU_PREP_STATE] = "3|Index player error",
[INDEX_GM_ERR__MENU_PREP_STATE] ="3|Game index error",
[MENU_INFO__MENU_PREP_STATE] = "3|%d|%d",

[NOT_IN_GAME__INIT_STATE] = "4|Client not in game",
[ALREADY_INDICATED_GAME__INIT_STATE] = "4|Already indicated",
[INDICATED_PL_GAME__INIT_STATE] = "4|Server received play indication",
[INDICATED_QUIT_GAME__INIT_STATE] = "4|Server received quit indication",

[NOT_IN_GAME__W_SCND_PL_STATE] = "5|Client not in game",
[NOT_INDICATED__W_SCND_PL_STATE] = "5|Not indicated to server",
[OTHER_PL_QUIT__W_SCND_PL_STATE] = "5|Other player indicated quit",
[OTHER_PL_NOT_IND__W_SCND_PL_STATE] = "5|Other player yet to indicate",
[OTHER_PL_PLAY_IND__W_SCND_PL_STATE] = "5|Other player indicated play",


[QUIT_STATMENT__QC_STATE] ="7|Server received client quit statment",

[QUIT_STATMENT__QS_STATE] = "8|Server initiating quit statment",

[DEFAULT] = "21|Error, invalid option"

};
*/
const char* error_string_holder[] = {
[NULL_POINTER] ="Null pointer is present",

[STRUCT_FIRST] = "First | was not found in message",
[NO_FIRST_NUMBER] = "first protocol number was not found",

[BUFF_OVF] ="Buffer overflow",

[MESS_FIRST_VALUE] = "Value of first protocol number was not correct",


};



const char* test_message_server[] = {
    [RANDOM_MESSAGE_TEST] = "1|Server reserved message",

    [QUIT_CLIENT_MESSAGE_TEST] = "7|Server received client quit statment",

    [FOUND_GAME_TEST] = "0|client found game",
    [CREATED_GAME_TEST] = "0|client created game",
    [GAMES_OCCUPIED_TEST] = "0|all games occupied",

    [MENU_PREPERATION_FAIL_GI] = "3|Client not in game",
    [MENU_PREPERATION_FAIL_PI]="3|Index player error",

    [NO_CREATED_GAME_WAIT] = "2|Client not in game",
    [SCND_PL_FOUND_WATT]  = "2|second player found",
    [SCND_PL_NOT_FOUND_WAIT] = "2|no second player",

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


	printf("Server responded with the following message: %s\n", buffer_message);

	if(strcmp(buffer_message, test_message_server[MENU_PREPERATION_FAIL_GI]) != 0){

		snprintf(buffer_error, BUFFER_SIZE, "We have the following error: %s",test_message_server[MENU_PREPERATION_FAIL_GI]);

		return handler_error(buffer_error);

	}

	if(strcmp(buffer_message, test_message_server[MENU_PREPERATION_FAIL_PI])!=0){

		snprintf(buffer_error, BUFFER_SIZE, "We have the following error: %s",test_message_server[MENU_PREPERATION_FAIL_PI]);

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

	printf("Server responded witht eh following message: %s\n", buffer_message);

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

    printf("Server responded with the following message: %s\n", buffer_message);

    const char* compare_buffer[3]={test_message_server[FOUND_GAME_TEST], test_message_server[CREATED_GAME_TEST], test_message_server[GAMES_OCCUPIED]};

    bool equal = compare_strings(buffer_message,compare_buffer, buffer_error, 3);

    if(!equal){

	return handler_error(buffer_error);

    }

    if(strcmp(test_message_server[FOUND_GAME_TEST]) != 0){

	printf("Server returned a valid message, but this test was specificly made to find a game, not create one or check if all games are occupied, we are going to exit now\n");

	return -1;

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

    const char* compare_buffer[3] = {test_message_server[CREATED_GAME_TEST], test_message_server[FOUND_GAME_TEST], test_message_server[GAMES_OCCUPIED_TEST]};
    bool equal = compare_strings(buffer_message,compare_buffer, buffer_error, 3);
    if(!equal){

	return handler_error(buffer_error);

    }

    if(strcmp(buffer_message, test_message_server[CREATED_GAME_TEST])!=0){

	printf("Server returned a valide, message, but this test was specificlyu made to create a game, not find an already created game or check if games are all occupied\n");

	return -1;

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

//this is called by the client that has created a game, and now it is waiting for the second player to enter
int test_wait_second_player_init(char*buffer_message,char* buffer_error,int client_file_descriptor){

	const int max_counter = 10; //remember that const does not mean that the expression that follows it is computed during compiliation, in c++ it is contsxrp

	int counter = 10;

	while(counter<max_counter){

		ssize_t result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);

		if(result == -1){

			return handle_error(buffer_error);

		}

		printf("Server responded with the following message: %s\n", buffer_message);

		if(strcmp(buffer_message, test_message_server[NO_CREATED_GAME_WAIT])==0){

			printf("Server indicatest that we have not created a game\n");

			//if we have not created a game, we are going to make the test suit quit

			return -1;

		}

		if(strcmp(buffer_message, test_message_server[SCND_PL_FOUND_WAT])==0){

			printf("We have found a second player\n");

			return 1; //one means that found second player;

		}


		if(strcmp(buffer_message, test_message_server[SCND_PL_NOT_FOUND_WAIT])==0){

			printf("Server indicates that a second player was not found\n We have %d iterations to go\n", max_counter-counter);

		}

		counter++;

	 }


	return 2; //this means that a second player was not found during the 10 different iterations


}
