#include <string.h>
#include <errno.h>

typedef enum{

	COMM_ERROR = 0, //error based on communication protocol

	INN_ERROR = 1 //error inner server, client does not have to do anything with these types of errors

}ERROR_LAYER_1;


typedef enum{

	SYSCALL_ERROR = 0,//possible syscall error 

	ABS_LEN_ERROR = 1 //length error on incoming message


}ERROR_LAYER_COMM;


typedef enum{

	ABS_OVERFLOW_BUFF = 0 //buffer overflow, server wants to send a message that is too big

	ABS_LEN_ERROR = 1, //lenght of message

	UD_POINTER = 2, //undefined pointer


}ERROR_LAYER_INN;

typedef struct{

	//to represent an error, we are going to need a buffer, that holds the message that is going to be printed to the standard ouput
	//aswell as two numbers that represent what type of error has occured

	char buffer_error[BUFFER_SIZE];

	int type1_error;

	int type2_error;

}error_node;

//for now we are going to leave it as a void pointer, we might change the error_node  data structure


void* main_error_handler(void* arg);
