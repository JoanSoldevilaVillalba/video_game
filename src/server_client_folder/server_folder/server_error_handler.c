#include "server_error_handler.h"

void* main_error_handler(void* arg){

	error_node* error = (error_node*)arg;

	//type1 helps us understand wheter we are in comm or inn:

	//we are going to use a swtich case, for scalibility

	switch(error->type1_error){

		case COMM_ERROR:

			//these types of errors happen when the communication has been broekn, syscalls and things taht the client has sent over taht are incorrect

			comm_handler(error);

			break;

		case INN_ERROR:

			//these are not communication based errors

			inn_handler(error);

			break;


		default:


			break;



	}



}



void* comm_handler(error_node* error){

	printf("---- We are in communication error handler ----\n");
	printf("Incoming error was due to the following: \n");
	switch(type2_error){

		case SYSCALL_ERROR:

			printf("Syscall failure, errno is giving us the following: %s , errno value: %d\n", strerror(errno), errno);

			//for this type of error, we are just going to have to close the connectoin and ask the user to communicate with the server another time

			break;


		case ABS_LEN_ERROR:

			printf("Incoming message length error\n");

			//in this case, i don't really know what i should do, should i create a new case in the while loop just for error handeling for comm?

			break;

	}



}


void* inn_handler(error_node* error){



}
