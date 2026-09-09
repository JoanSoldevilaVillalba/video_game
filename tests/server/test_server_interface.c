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
    [MAX_MESSAGES_TEST] = NULL
};

int test_quit_client(char* buffer_message, char* buffer_error, int file_descriptor) {
    printf("\n ----- testing quit statement from client ------ \n");
    int result = send_validated_message(buffer_message, buffer_error, file_descriptor);
    if (result == -1) {
        printf("An error occurred: %s\n", buffer_error);
        printf("Testing is quitting now .... \n");
        return -1;
    }

    printf("We have succesfully sent the following message: %s", buffer_message);
    result = receive_validated_message(buffer_message, buffer_error, file_descriptor);
    if (result == -1) {
        printf("An error occurred: %s\n", buffer_error);
        printf("Testing is quitting now .... \n");
        return -1;
    }

    printf("Server has responded with the following message: %s\n", buffer_message);
    printf("Expected message: %s\n", test_message_server[QUIT_CLIENT_MESSAGE_TEST]);

    if (strcmp(buffer_message, test_message_server[QUIT_CLIENT_MESSAGE_TEST]) != 0) {
        printf("An error occurred\n");
        printf("Expected message is the following: %s", test_message_server[QUIT_CLIENT_MESSAGE_TEST]);
        return -1;
    }

    printf("Test was completed correctly\n");
    return result;
}

int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor) {
    printf("\n------- testing receive message ------- \n");
    int result = receive_validated_message(buffer_message, buffer_error, file_descriptor);
    if (result == -1) {
        printf("An error has occuried: %s\n", buffer_error);
        printf("We are quitting the test, goodbye ...\n");
        return -1;
    }

    printf("Server has sent over the following message: %s\n", buffer_message);
    if (strcmp(buffer_message, test_message_server[RANDOM_MESSAGE_TEST]) != 0) {
        printf("An error has occuried\n");
        printf("Recevied message from server is not equal to the following string: %s", test_message_server[RANDOM_MESSAGE_TEST]);
        printf("We are qutting the tests, goodye ....\n");
        return -1;
    }

    printf("this test was a sucess \n");
    return result;
}

int test_setup_connection(char* buffer_error, int server_port) {
    struct sockaddr_in server_address;
    int file_descriptor = -1;

    printf("----- Testing Connection Setup -----\n");
    int result = setupConnection(&file_descriptor, &server_address, server_port, buffer_error);
    if (result < 0) {
        printf("Something went wrong: %s\n", buffer_error);
        printf("We are quitting the test, goodbye....\n");
        return -1;
    }

    printf("Setup connection was a success\n");
    return file_descriptor;
}

int test_create_game_client(char* buffer_message, char* buffer_error, int client_file_descriptor) {
    printf("\n ----- testing create game action ----- \n");
    int result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);
    if (result == -1) {
        printf("An error has occuried: %s\n", buffer_error);
        printf("We are quitting the test, goodbye ...\n");
        return -1;
    }

    printf("Test client has succesfully sent the following message: %s\n", buffer_message);
    result = receive_validated_message(buffer_message, buffer_error, client_file_descriptor);
    if (result == -1) {
        printf("An error has occuried: %s\n", buffer_error);
        printf("We are quitting the test, goodbye ...\n");
        return -1;
    }

    printf("Server has returned the following statment: %s\n", buffer_message);
    printf("These are the possible outcomes:\n");

    for (int i = 0; test_message_server[i] != NULL; i++) {
        printf("%s\n", test_message_server[i]);
        if (strcmp(buffer_message, test_message_server[i]) == 0) {
            printf("We have found a match, done\n");
            return 0;
        }
    }

    printf("Unable to find matching message\n We are quitting the tests now, goodbye\n");
    return -1;
}

int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor) {
    printf("\n------ testing message sending ------\n");
    ssize_t result = send_validated_message(buffer_message, buffer_error, client_file_descriptor);
    if (result == -1) {
        printf("Something went wrong: %s\n", buffer_error);
        printf("We are qutting the test, goodbye ...\n");
        return result;
    }

    printf("Sending a message was a success\n");
    return 0;
}
