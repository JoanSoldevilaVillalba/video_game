#include "test_server_1.h"

typedef enum{

TEST_CONNECTION_FD_ERROR = 0,

TEST_CONNECTION_FRMT_ERROR = 1,

TEST_CONNECTION_CNT_ERROR = 2,

TEST_NULL_POINTER_ERROR = 3

}testID;

const char* test_string_holder[]={

[TEST_CONNECTION_FD_ERROR] = "Error, no file descriptor was assigned to client: %s",
[TEST_CONNECTION_FRMT_ERROR] = "Error, possible incorrect format: %s",
[TEST_CONNECTION_CNT_ERROR] = "Error, connection with server was not possible: %s",
[TEST_NULL_POINTER_ERROR] = "Error, pointer is null"

};


int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port, char* buffer_error){

	if(client_file_descriptor == NULL || server_address == NULL || buffer_error == NULL){

		snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_NULL_POINTER_ERROR]);

	}

        *(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);

        if(*(client_file_descriptor)<0){

		snprintf(buffer_error, BUFFER_SIZE,test_string_holder[TEST_CONNECTION_FD_ERROR], strerror(errno));

                return -1;

        }


        server_address->sin_family = AF_INET;

        server_address->sin_port = htons(port);


        int result_translation = 0;

        int status = 0;


        result_translation =  inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

        if(result_translation <=0){

		snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_CONNECTION_FRMT_ERROR], strerror(errno));

		close(*(client_file_descriptor));

		*(client_file_descriptor) = -1;

                return -1;

        }

	struct timeval timeout = {.tv_sec = 2, .tv_usec = 0};

	setsockopt(*client_file_descriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));


        if(status<0){

		snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_CONNECTION_CNT_ERROR], strerror(errno));

		close(*(client_file_descriptor));

		*client_file_descriptor = -1;

                return -1;

        }


        return 0;

}

int test_setup_connection(char* buffer_error, int server_port){

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

bool validate_message_length(const char* temporary_pointer, char* buffer_error){

        if(BUFFER_SIZE <= strlen(temporary_pointer)){

		snprintf(buffer_error, BUFFER_SIZE, 

                return false;

        }else{

                return true;

        }

        //rememeber that strlen is indexed to one,

}

ssize_t send_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor){

	bool correct_length = validate_message_length(temporary_pointer, buffer_error);

	ssizse_t result = 0;

	if(correct_length == true){

		result = send_framed_message(client_file_descriptor, buffer, (uint32_t)strlen(temporary_pointer));

		if(result == -1){

			return -1;

		}

		return 0;

	}else{


		return -1;

	}


}

int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor){

	printf("\n------ testing message sending ------\n")

	int result = -1;



	if(result == -1){

		printf("Something went wrong: %s\n", buffer_error);

		printf("We are qutting the test, goodbye ...\n");

		return result;

	}

	printf("Sending a message was a success\n");

	return 0;

}

int main(){

	int port = 8080;

	char buffer_send[BUFFER_SIZE];

	char buffer_receive[BUFFER_SIZE];

	char buffer_error[BUFFER_SIZE];


	int client_file_descriptor = test_setup_connection(buffer_error, port);

	if(client_file_descriptor <0){

		return client_file_descriptor;

	}

	int message_send_result = test_send_message(buffer_send, buffer_error, client_file_descriptor);

	if(message_send_result <0){

		return message_send_result;

	}

	return 0;

}
