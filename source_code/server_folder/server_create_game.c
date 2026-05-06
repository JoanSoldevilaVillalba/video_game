//this file is going to contain the server code
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
//for now we are juist going to be using port number 8080

/*int bind_socket(int server_fd, struct sockaddr_in* address, int port){

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
}*/

//we should probably send back some type of menu:


void handle_client(int temporary_fd){
//this function will be called everytime that the server has to handle a client. We can probabnly use threds instead of functions here, because it is important to be able to execute different clients symotanouilsy
//keep in mind that this is just for tcp, when handeling udp packages, meaning when we are in game and have to send ifnormatoin,  we are going to use a different methode.
//we are just going to need a file descriptor
printf("\nSERVER CONNECTED TO CLIENT WITH FD: %d\n", new_socket);
if(temporary_fd<0){

printf("something went wrong, we were not able to create a socket or assigne file descriptor");

exit(EXIT_FAILURE);

}
char buffer_receive[1024];
char buffer_send[1024] = "HELLO THIS IS THE SERVER";
int bytes_received = read(new_socket, buffer_receive, 1023);
printf("CLIENT NUMBER %d HAS SENT THE FOLLOWING MESSAGE: %s\n\n",new_socket, buffer_receive);
//now we are going to have to interpret what it wants:
send(new_socket,buffer_send, strlen(buffer_send),0);
close(new_socket);

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
char buffer_receive[1024];
char buffer_send[1024] = "HELLO FROM SERVER";

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

if (bind(server_file_descriptor, (struct sockaddr*)&address,sizeof(address))< 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


printf("THIS IS THE SERVER\n THE SERVER HAS THE FOLLOWING INFORMATION\n");
printf("SERVER FAMILY: %hd\n", address.sin_family);
printf("SERVER PORT NUMBER: %hu\n",ntohs(address.sin_port));
printf("\nTHE SERVER IS READY\n\n\n");

listen(server_file_descriptor,2);

while(1){
new_socket = accept(server_file_descriptor, (struct sockaddr*)&address,(socklen_t*)&addrlen);
printf("\nSERVER CONNECTED TO CLIENT WITH FD: %d\n", new_socket);
if(new_socket<0){

printf("something went wrong, we were not able to create a socket or assigna file descriptor");

exit(EXIT_FAILURE);

}
int bytes_received = read(new_socket, buffer_receive, 1023);
printf("CLIENT NUMBER %d HAS SENT THE FOLLOWING MESSAGE: %s\n\n",new_socket, buffer_receive);
send(new_socket,buffer_send, strlen(buffer_send),0);
close(new_socket);


}
close(server_file_descriptor);
}
