#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int main(int argc, char const* argv[]){

int status = 0;
int client_file_descriptor = 0;
struct sockaddr_in server_addr;
char send_buffer[1024];
char receive_buffer[1024];


//we are going to have to create the socket
client_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
if(client_file_descriptor<0){
printf("error in creating a socket / file descritpro\n");
return 1;
}




return 0;
}
