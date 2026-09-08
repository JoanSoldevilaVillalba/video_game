#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "curl/curl.h"

#define BUFFER_SIZE 64

#if defined(_WIN32) || defined(_WIN64)
    #define GLOBAL_INIT_BIT CURL_GLOBAL_WIN32
#else
    #define GLOBAL_INIT_BIT CURL_GLOBAL_ALL
#endif


struct MemoryStruct{

char* memory;

size_t size;

};

typedef  enum{

DUCK_DNS_URL = 0,

DUCK_DNS_DOMAIN = 1,

DUCK_DNS_TOKEN = 2


}webID;

const char* string_web_holder[]={
[DUCK_DNS_URL]"https://www.duckdns.org/update?domains=%s&token=%s&verbose=true",
[DUCK_DNS_DOMAIN] = "video-game-project",
[DUCK_DNS_TOKEN]="2ff8342c-836c-4712-aa25-ff5c90cd43a5"
};

static size_t write_callback(char *content_received, size_t size, size_t nmemb, void *user_type_data){

	size_t realsize = size * nmemb;

	struct MemoryStruct* mem = ( struct MemoryStruct*)user_type_data;

	char* temporary_ptr = realloc(mem->memory, mem->size + realsize +1);

	if(temporary_ptr == NULL){

//		snprintf(mem->buffer_error, BUFFER_SIZE, "%s", "Error, not enough memory for received transfer");

		return 0;

	}

	mem->memory = temporary_ptr;

	memcpy(&mem->memory[mem->size], content_received, realsize);

	mem->size += realsize;

	mem->memory[mem->size] = 0;

	printf("We have receveid the following amount of bytes: %d\n",(int)(size * nmemb));

	return size * nmemb; //amount of bytes that this call of write_callback has received

}


int main(void){

	char buffer_url[BUFFER_SIZE+BUFFER_SIZE];

	char buffer_domain[BUFFER_SIZE];

	char buffer_token[BUFFER_SIZE+BUFFER_SIZE];

	char buffer_error[CURL_ERROR_SIZE];

	snprintf(buffer_domain, BUFFER_SIZE, "%s", string_web_holder[DUCK_DNS_DOMAIN]);

	snprintf(buffer_token, BUFFER_SIZE+BUFFER_SIZE, "%s", string_web_holder[DUCK_DNS_TOKEN]);

	snprintf(buffer_url, BUFFER_SIZE+BUFFER_SIZE,string_web_holder[DUCK_DNS_URL], buffer_domain, buffer_token);

	printf("Here we have the url that we are going to retreive data from : %s\n", buffer_url);


	struct MemoryStruct user_data;

	user_data.memory = malloc(1);

	user_data.size = 0;

	CURL* handler = NULL;

	CURLcode res;

	printf("size of curl error size: %d", (int)CURL_ERROR_SIZE);


	curl_global_init(GLOBAL_INIT_BIT);

		handler = curl_easy_init();

			if(handler){

				curl_easy_setopt(handler, CURLOPT_URL,buffer_url);

				curl_easy_setopt(handler,CURLOPT_VERBOSE, 1L);

				curl_easy_setopt(handler, CURLOPT_WRITEDATA, (void*)&user_data);

				curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, write_callback);

				curl_easy_setopt(handler, CURLOPT_ERRORBUFFER, buffer_error);

				res = curl_easy_perform(handler);

				if(res!=CURLE_OK){

					printf("Error calling curl_easy_perform: %s, extra: %s\n", curl_easy_strerror(res), buffer_error);

				}else{


					printf("The transfer was a success\n");

					printf("This is the content: \n %s\n\n", user_data.memory);

					char* domain = NULL;

					const char* delimiter = "domain";

					domain = strstr(user_data.memory, delimiter);

					if(domain){

						printf("We have found the domain. Here is the address: %p", domain);

					}else{

						printf("We were not able to find the following substring: %s", delimiter);

					}


				}

			curl_easy_cleanup(handler);

			}else{
				printf("Error, curl_easy_init faild, returned null pointer\n");
			}

	free(user_data.memory);

	curl_global_cleanup();


	return 0;
}
