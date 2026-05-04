#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
//keep in mind that we have separated the action to send infroamtion to the server with the following functions.
//we are doing this just for readbibliy, and here speed is not needed, so adding function overhead is not really a problem, we probably won't do this for udp/sending packages with in game data thought. this implemetnaiton is just to connect to the server.


int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port){

//asining a file descirptor to ourselves:
*(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);

//heree we are just simply defining what version of the ip protoocl the server address is using aswellas the port where the server porgram is runing in in the other computer.

server_address->sin_family = AF_INET;
server_address->sin_port = htons(port);

//now we are going to have to convert the loop back address to a usable binary address taht the operating system is able to use to store in sin_addr of server_address
int result_translation= inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

int status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));

//we are going to have to add safe guards and error control, but for now we are going to leave it like this

}
int send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor){

int bytes_sent = send(client_file_descriptor, buffer_r,strlen(buffer_r),0);
int bytes_read = read(client_file_descriptor,buffer_s, strlen(buffer_s));

//we have not really defined yet how the protocol is going to work, so for now we are not going to close the connection that we have established with the server.
//maybe it is better to keep the connection alive, but this is up to the protocol that we have not yeet defined
//close(client_file_descriptor)
}

/*
in this function we are going to estblish the tcp connection with the server
it returns the connection, the status of the connection
to be able to establish the connection we need the following parameters:

client_file_descritpro, server address and that is it, for sending the message, that will be separate function

*/

