#include "test_server_interface.h"
#include "test_server_logic.h"

const char* error_string_holder[] = {
[NULL_POINTER] ="Null pointer is present",

[STRUCT_FIRST] = "First | was not found in message",
[NO_FIRST_NUMBER] = "first protocol number was not found",

[BUFF_OVF] ="Buffer overflow",

[MESS_FIRST_VALUE] = "Value of first protocol number was not correct"


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
    [SCND_PL_FOUND_WAIT]  = "2|second player found",
    [SCND_PL_NOT_FOUND_WAIT] = "2|no second player",


    [NOT_IN_GAME_INIT]="4|Client not in game",
    [ALREADY_INDICATE] = "4|Already indicated",
    [INDICATED_PL] = "4|Already indicated",
    [INDICATED_QT] ="4|Server received quit indication",

    [OTHER_NOT_INDICATE] ="5|Other player yet to indicate",
    [OTHER_INDICATED_PL] = "5|Other player indicated play",
    [OTHER_INDICATED_QT]= "5|Other player indicated quit",
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

    const char* compare_buffer[3]={test_message_server[FOUND_GAME_TEST], test_message_server[CREATED_GAME_TEST], test_message_server[GAMES_OCCUPIED_TEST]};

    bool equal = compare_strings(buffer_message,compare_buffer, buffer_error, 3);

    if(!equal){

	return handler_error(buffer_error);

    }

    if(strcmp(buffer_message, test_message_server[FOUND_GAME_TEST]) != 0){

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

	return handler_error(buffer_error);

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

	printf("\n------ testing waiting for second player mechanic (just after creating the game) ------\n");

	const int max_counter = 10; //remember that const does not mean that the expression that follows it is computed during compiliation, in c++ it is contsxrp

	int counter = 0;

	ssize_t result = -1;

	while(counter<max_counter){

		result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);

		if(result == -1){

			return handler_error(buffer_error);

		}

		result = receive_validated_message(buffer_message, buffer_error, client_file_descriptor);

		if(result == -1){

			return handler_error(buffer_error);

		}

		if(strcmp(buffer_message, test_message_server[NO_CREATED_GAME_WAIT])==0){

			printf("Server indicatest that we have not created a game\n");

			//if we have not created a game, we are going to make the test suit quit

			return -1;

		}

		if(strcmp(buffer_message, test_message_server[SCND_PL_FOUND_WAIT])==0){

			printf("We have found a second player\n");

			return 1; //one means that found second player;

		}


		if(strcmp(buffer_message, test_message_server[SCND_PL_NOT_FOUND_WAIT])==0){

			printf("Server indicates that a second player was not found\n We have %d iterations to go\n", max_counter-counter);

		}

		printf("Server responded with the following message: %s\n", buffer_message);

		//we are just going to sleep for 3 seconds();

		printf("%d number done\n", counter);

		sleep(1);

		counter++;

	 }


	return -1;


}

int test_indicate_play(char* buffer_message, char* buffer_error, int client_file_descriptor){

	ssize_t result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);

	if(result == -1){

		return handler_error(buffer_error);

	}

	result = receive_validated_message(buffer_message, buffer_error, client_file_descriptor);

	if(result == -1){

		return handler_error(buffer_error);

	}

	printf("Server responded with the following message: %s\n", buffer_message);

	if(strcmp(buffer_message, test_message_server[NOT_IN_GAME_INIT]) == 0){

		printf("Message recevied from server is a possible valid message, but not what we were looking for\n");

		return -1;

	}

	if(strcmp(buffer_message, test_message_server[ALREADY_INDICATE]) == 0){

		printf("Message received from server is a possible valid mesage, but not what we were looking for\n");

		return -1;

	}

	if(strcmp(buffer_message, test_message_server[INDICATED_QT])==0){

		printf("Error in messaging protocol, we tried to indicate that we want to play not quit\n");

		return -1;

	}


	if(strcmp(buffer_message, test_message_server[INDICATED_PL])!=0){

		printf("Something very wrong happened\n");

		return -1;

	}


	printf("Server responded with the following message: %s", buffer_message);




}


int test_wait_other_indicate(char* buffer_message, char* buffer_error, int file_descriptor){

	const int max_counter = 5;

	int counter = 0;

	ssize_t result = 0;

	while(counter<max_counter){

		result = send_validated_message(buffer_message, buffer_error, file_descriptor);

		if(result == -1){

			return handler_error(buffer_error);

		}

		result = receive_validated_message(buffer_message, buffer_error, file_descriptor);

		printf("Server responded with the following message: %s\n", buffer_message);

		if(result == -1){

			return handler_error(buffer_error);

		}

		if(strcmp(buffer_message,test_message_server[OTHER_NOT_INDICATE])==0){

			printf("The other player has not yet indicated, we have %d iterations to go", max_counter-counter);

		}else if(strcmp(buffer_message, test_message_server[OTHER_INDICATED_PL])==0){

			printf("The other player as indicated that he or she is also going to play\n");

			return result;

		}else if(strcmp(buffer_message, test_message_server[OTHER_INDICATED_QT])==0){

			printf("Possible message was recieved but not the expected message\n");

			return -1;
		}else{

			printf("Either we were not in a game or we have still yet to indicate. This was not the expected message\n");

			return -1;

		}

		sleep(1);

		counter++;


	}

	return -1;

}
