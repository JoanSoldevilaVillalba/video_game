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
//the following we have the message that we want to send:
char* send_message="This is joan soldevila vilalba";

//we are going to have to create the socket
client_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
if(client_file_descriptor<0){
printf("error in creating a socket / file descritpro\n");
return 1;
}

//WE HAVE now crated the socket, and now have the file descriptor to this socket.
//nmwow we have to define the ip address for the server:
if(inet_pton(AF_INET, "127.0.0.1",&server_addr.sin_addr)<0){
//this function converst the loop back address to a binary usable address which is usable.
printf("the address is not valid\n");
return 2;
}



status = connect(client_file_descriptor, (struct sockaddr*)&server_addr, sizeof(server_addr));
if(status<0){
printf("Error, we were not able to connect, soemthing went wrong with tryihng to connect to the server\n");
return 32;
}
int n_bytes_succes = send(client_file_descriptor, send_message, sizeof(send_message),0);
if(n_bytes_succes<=0){
printf("Error, we were not able to send the bytes\n");
return 342;
}

printf("message was sent correcly\n");

int return_bytes = read(client_file_descriptor, receive_buffer, sizeof(receive_buffer));
if(return_bytes<=0){
printf("Error, no bytes where receives from sergver\n");
return 67;
}
printf("message was receives with succes\n");

return 0;
}
