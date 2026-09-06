#include "test_server_1.h"

const char* test_string_holder[]={

[TEST_CONNECTION_FD_ERROR] = "Error, no file descriptor was assigned to client: %s",
[TEST_CONNECTION_FRMT_ERROR] = "Error, possible incorrect format: %s",
[TEST_CONNECTION_CNT_ERROR] = "Error, connection with server was not possible: %s",


[TEST_NULL_POINTER_ERROR] = "Error, pointer is null",


[TEST_SEND_TIMEOUT_ERROR] = "Error on setting timeout on client socket when sending information: %s",
[TEST_SEND_RESET_TMO_ERROR] = "Error on resetting timeout to zero on client socket: %s",
[TEST_SEND_LENGTH_INIT_ERROR] = "Error, the amount of bytes that were sent in init message is not equal to theoretical value",
[TEST_SEND_LENGTH_MESS_ERROR] = "Error, the amount of bytes that were sent in the real message is not equal to its actual size (original message size not equal to bytes sent)",
[TEST_SEND_SYS_CALL_ERROR] = "Error, syscall error has occurred: %s",

[TEST_MESSAGE_LENGTH_ERROR] = "Error, message length is too long, overflow",

[TEST_RECV_VALD_STRUCT_FRST_ERROR] = "Error, first seperator is missing in the following message: %s",
[TEST_RECV_VALD_STRUCT_SCND_ERROR] = "Error, second seperator is missing in the following message: %s",
[TEST_RECV_VALD_NN_FRST_ERROR] = "Error, there is no first number: %s",
[TEST_RECV_VALD_NN_SCND_ERROR] = "Error, there is no second number: %s",
[TEST_RECV_VALD_VALUE_FRST_ERROR] = "Error, first value of message is not correct",
[TEST_RECV_VALD_VALUE_SCND_ERROR]= "Error, second value of message is not correct",
[TEST_RECV_SYSCALL_ERROR] = "Error, syscall error when receving bytes from server: %s",
[TEST_RECV_TIMEOUT_ERROR] = "Error on setting timeout on client socket when receving information: %s",
[TEST_RECV_RESET_TMO_ERROR]="Error on resetting timeout to zero on client socket when receving information: %s",
[TEST_RECV_NO_BYTES_ERROR] = "Error, no byte were recevied from the server",
[TEST_RECV_INIT_LENGTH_ERROR] = "Error, the amount of bytes that were recevied in init message is not equal to theoretical value",
[TEST_RECV_MESS_LENGTH_ERROR] = "Error, the amount of bytes that were received in the real message is not equal to its actual size",
[TEST_RECV_MAX_LENGTH_ERROR] = "Error, init message indicates message is to large for client, possible overflow",
};

const char* test_message_server[]={

[RANDOM_MESSAGE_TEST] = "2|1|this is response to random message"

};
ssize_t read_all(int temporary_fd, char buffer[], ssize_t length, char* buffer_error){

        ssize_t total_length  = 0;

        ssize_t n = 0;

        while(total_length < length){

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);

                if(n<0){


                        if(errno == EINTR){
                                continue;
                        }

			snprintf(buffer_error, BUFFER_SIZE,test_string_holder[TEST_RECV_SYSCALL_ERROR], strerror(errno));

                        return -1;

                }

                if(n == 0){
                        break;
                }
                total_length+=n;

        }

        return total_length;

}

ssize_t receive_framed_message(int fd, char* buf, ssize_t max_buf_len, char* buffer_error){

        uint32_t net_len = 0;

        ssize_t header_bytes = 0;

        struct timeval timeout;

        timeout.tv_sec = 5;

        timeout.tv_usec = 0;

        if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))==-1){

		snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_RECV_TIMEOUT_ERROR], strerror(errno));

                return -1;

        }

        header_bytes = read_all(fd, (char*)&net_len, (ssize_t)sizeof(net_len), buffer_error);

        if(header_bytes == 0){

		snprintf(buffer_error, BUFFER_SIZE,"%s", test_string_holder[TEST_RECV_NO_BYTES_ERROR]);

                return -1;

        }

        if(header_bytes == -1){

                return -1;

        }

        if(header_bytes< (ssize_t)sizeof(net_len)){

		snprintf(buffer_error, BUFFER_SIZE,"%s", test_string_holder[TEST_RECV_INIT_LENGTH_ERROR]);

                return -1;

        }


        uint32_t payload_len = ntohl(net_len);

        if((ssize_t)payload_len >= max_buf_len){

		snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_RECV_MAX_LENGTH_ERROR]);

                return -1;

        }


        ssize_t payload_bytes = read_all(fd, buf,(ssize_t)payload_len, buffer_error);

        timeout.tv_sec = 0;

        timeout.tv_usec = 0;

        if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))==-1){

		snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_RECV_RESET_TMO_ERROR], strerror(errno));

                return -1;

        }

        if(payload_bytes == -1){

                return -1;

        }

        if(payload_bytes < (ssize_t)payload_len){

		snprintf(buffer_error, BUFFER_SIZE,"%s", test_string_holder[TEST_RECV_MESS_LENGTH_ERROR]);

                return -1;
        }

        buf[payload_len]='\0';

        return (ssize_t)payload_len;

}


bool check_structure(char* buffer_message, char*buffer_error){

	 if(buffer_message == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_NULL_POINTER_ERROR]);

                return false;

        }

        char* first_sep=strchr(buffer_message, '|');

        if(first_sep ==NULL){

                snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_RECV_VALD_STRUCT_FRST_ERROR], buffer_message);

                return false;

        }

        char* second_sep = strchr(first_sep + 1, '|');

        if(second_sep == NULL){

                snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_RECV_VALD_STRUCT_SCND_ERROR], buffer_message);

                return false;

        }

        return true;


}


bool check_numbers(char* buffer_message, char* buffer_error){

        char* first = buffer_message;

        char* end = NULL;

        const char delimiter = '|';

        int first_number = -1, second_number = -1;

        if(buffer_message == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_NULL_POINTER_ERROR]);

                return false;

        }


        end = strchr(first, delimiter);

        if(end == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_RECV_VALD_STRUCT_FRST_ERROR]);

                return false;

        }

        if(first == end){

                snprintf(buffer_error, BUFFER_SIZE,test_string_holder[TEST_RECV_VALD_NN_FRST_ERROR], buffer_message);

                return false;

        }

        first_number = (int)strtol(first, &end,10);


        first = end + 1;

        end = first;

        end = strchr(first, delimiter);

        if(end == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_RECV_VALD_STRUCT_SCND_ERROR]);

                return false;

        }

        if(end == first){

                snprintf(buffer_error, BUFFER_SIZE,test_string_holder[TEST_RECV_VALD_NN_SCND_ERROR], buffer_message);

                return false;

        }

        second_number = (int)strtol(first, &end, 10);

        if(first_number<0 || first_number>10){

                snprintf(buffer_error, BUFFER_SIZE, "%s",test_string_holder[TEST_RECV_VALD_VALUE_FRST_ERROR]);

                return false;

        }


        if(second_number<0||second_number>10){

                snprintf(buffer_error,BUFFER_SIZE,"%s",test_string_holder[TEST_RECV_VALD_VALUE_FRST_ERROR]);

                return false;

        }

        return true;

}

int receive_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor){

	ssize_t result = receive_framed_message(client_file_descriptor, buffer_message, (ssize_t)BUFFER_SIZE, buffer_error);

	if(result == -1){

		return -1;

	}

	if(!check_numbers(buffer_message, buffer_error) || check_structure(buffer_message, buffer_error)){

	return -1;

	}

	return (int)result;//converting ssize_t to int

}

int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor){

	printf("\n------- testing receive message ------- \n");

	int result = -1;

	result = receive_validated_message(buffer_message, buffer_error, file_descriptor);

	if(buffer_message){

		printf("Server has sent over the following message: %s", buffer_message);

		printf("The expected message was the following: %s", test_message_server[RANDOM_MESSAGE_TEST]);

	}

	if(result == -1){

		printf("An error has occuried: %s\n", buffer_error);

		printf("We are quitting the test, goodbye ...\n");

		return -1;
	}

	printf("this test was a sucess \n");

	return result;


}

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

		snprintf(buffer_error, BUFFER_SIZE,"%s", test_string_holder[TEST_MESSAGE_LENGTH_ERROR]);

                return false;

        }else{

                return true;

        }

        //rememeber that strlen is indexed to one,

}
ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length, char* buffer_error){

        ssize_t total_length  = 0;

        ssize_t n = 0;


        while(total_length<length){

                n = send(temporary_fd,buffer+total_length,length - total_length,0);

                if(n<0){

                        if(errno == EINTR){

                                continue;
                        }

			snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_SEND_SYS_CALL_ERROR], strerror(errno));

                        return -1;

                }

               if(n == 0){

                        break;

                }

                total_length+=n;

        }

        return total_length;

}
ssize_t send_framed_message(int fd,char* buffer_message , uint32_t payload_len, char* buffer_error) {

        uint32_t net_len = htonl(payload_len);

        struct timeval timeout;

        timeout.tv_sec = 5;

        timeout.tv_usec = 0;


        if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1){

		snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_SEND_TIMEOUT_ERROR], strerror(errno));

                return -1;

        }


        ssize_t result = send_all(fd, (const char *)&net_len, sizeof(net_len), buffer_error);

        if(result == -1){

                return -1;

        }

        if ( result != sizeof(net_len)) {

		snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_SEND_LENGTH_INIT_ERROR]);

                return -1;

        }

        result = send_all(fd,buffer_message, payload_len, buffer_error);

        timeout.tv_sec = 0;

        timeout.tv_usec = 0;

        if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1){

		snprintf(buffer_error, BUFFER_SIZE, test_string_holder[TEST_SEND_RESET_TMO_ERROR], strerror(errno));

                return -1;

        }


        if(result == -1){

                return -1;

        }

        if (result != (ssize_t)payload_len) {

		snprintf(buffer_error, BUFFER_SIZE, "%s", test_string_holder[TEST_SEND_LENGTH_MESS_ERROR]);

                return -1;

        }

    return (ssize_t)(sizeof(net_len)) + (ssize_t)payload_len;
}


ssize_t send_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor){

	bool correct_length = validate_message_length(buffer_message, buffer_error);

	ssize_t result = 0;

	if(correct_length == true){

		result = send_framed_message(client_file_descriptor, buffer_message, (uint32_t)strlen(buffer_message), buffer_error);

		if(result == -1){

			return -1;

		}

		return 0;

	}else{

		return -1;

	}


}

int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor){

	printf("\n------ testing message sending ------\n");

	ssize_t result = -1;

	result =  send_validated_message(buffer_message,buffer_error,client_file_descriptor);


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

	snprintf(buffer_send, BUFFER_SIZE, "%s", "2|0|random message init"); //this is the random message that we are going to have to send to the server side

	int message_send_result = test_send_message(buffer_send, buffer_error, client_file_descriptor);

	if(message_send_result <0){

		return message_send_result;

	}

	int message_receive_result =  test_receive_message(buffer_receive,buffer_error, client_file_descriptor);


	return 0;

}
