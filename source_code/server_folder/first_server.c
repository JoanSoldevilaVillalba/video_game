//this file is going to contain the server code
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

int bind_socket(int server_fd, struct sockaddr_in* address, int port){

	bool quit = false;
	int return_value = 0;
	while(!quit){

		if(bind(server_fd, (struct sockaddr*)address, sizeof(*(address)))<0){

			if(errno == EADDRINUSE){

				address->sin_port = htons(++port);

			}else{

			quit = true;
			return_value = 1;
			}
		}else{
		quit = true;
		}
	}
return return_value;
}

int main(int argc, char const* argv[])
{

int server_file_descriptor = 0;
int new_socket = 0;
ssize_t valread;
struct sockaddr_in address;
int opt = 1;

int port_number =8080;

socklen_t addrlen = sizeof(address);
char buffer[1024] = {0};
char* hello = "Hello from server";

//we are first going to create the socket  by calling socket, with AF_INET(IPv4) using IP protocol version 4, SOCK_STREAM (tcp)
server_file_descriptor = socket(AF_INET, SOCK_STREAM,0);
//file descritpro number zero is usually occupied, but we are still going to be following the tutorial
if(server_file_descriptor<0){

printf("Error, the operating system was not able to assign us a file descriptor for the tcp ip socket that we wanted to create\n");
return 1;
}

//inthe folloinwg line we are going to bind a created socket to a port, in this case the port is 8080
//in the following code, we are using SO_REUSEADDR, which is a flag that tells the operating system to assign us the port even thought it was recently in use(TIME_WAIT) or there is an active program
//that is using the port but also has the SDO_REUSEADDRE flag activated, allowing our server to use this port.
//SO_REUSEADDRE-> is what allows us to bind the server to an already used port (TIME_WAIT)
//SO_REUSEPORT-> is what allows use to bin multiple sockets to the same port
//keep in mind that we are just setting up the socket 
if(setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){

printf("for some reason there was an error with configuring the socket\n");
return 2;

}
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(port_number);

int success_bind = bind_socket(server_file_descriptor, &address, port_number);
//remember that from now on port_number is not usable, we need to direcly access address.sin_port, because we are passing a copu of port_number not its address.

if(success_bind == 1){
printf("error, unable to find a usable address\n");
return 3;
}
//we have now implemented the logic for creating a socket and binding it to a usbale port.

}
