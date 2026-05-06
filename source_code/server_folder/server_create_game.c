//this file is going to contain the server code
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

void parse_received_data(char*r_buffer, ssize_t size_buffer,char* s_buffer ){

}

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

server_file_descriptor = socket(AF_INET, SOCK_STREAM,0);
if(server_file_descriptor<0){

printf("Error, the operating system was not able to assign us a file descriptor for the tcp ip socket that we wanted to create\n");
return 1;
}

if(setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){

printf("for some reason there was an error with configuring the socket\n");
return 2;

}
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(port_number);

int success_bind = bind_socket(server_file_descriptor, &address, port_number);

if(success_bind == 1){
printf("error, unable to find a usable address\n");
return 3;
}

listen(server_file_descritpor,2); 

while(1){

new_socket = accept(server_file_descriptor, (struct sockaddr*)&address,(socklen_t*)&addrlen);

if(new_socket<0){

printf("something went wrong, we were not able to create a socket or assigna file descriptor");

exit(EXIT_FAILURE);

}
char receive_buffer[1024];
char send_buffer[1024];
ssize_t bytes_received = recv(new_socket, receive_buffer, sizeof(receive_buffer), 0);
if(bytes_received<0){
printf("Error, for some reason there was an error with receiving the data from the client\n");
return 4;
}

parse_received_data(receive_buffer, bytes_received, send_buffer);

close(new_socket);


}
close(server_file_descriptor);
