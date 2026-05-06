#include "client_create_game.h"
void setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port){

*(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);


server_address->sin_family = AF_INET;
server_address->sin_port = htons(port);

int result_translation= inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

int status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));

}
int send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor){

int bytes_sent = send(client_file_descriptor, buffer_s,strlen(buffer_r),0);
int bytes_read = read(client_file_descriptor,buffer_r, 1023);
if(bytes_read>=0 && (bytes_read-1)){
buffer_r[bytes_read] = '\0';
}
return bytes_read;
}


void handleServerCommunication(int server_port){
	char buffer_send[1024] = "HELLO";
	char buffer_receive[1024];
	struct sockaddr_in server_address;
	int bytes_receive = 0;
	int bytes_send = 0 ;
        bool quit = false;
        int option = 0;
	int client_file_descriptor = 0;

	setupConnection(&client_file_descriptor, &server_address, server_port);

	bytes_receive = send_data_to_server(buffer_receive, buffer_send, client_file_descriptor);
	printf("we have received %d bytes\n", bytes_receive);
	printf("the server responded with the following: %s\n", buffer_receive);
	close(client_file_descriptor);
}
int main(){


printf("This is the client interface\n");
int port = 8080;
handleServerCommunication(port);


}
