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

char buffer_error[BUFFER_SIZE];

};

static size_t write_callback(char *content_received, size_t size, size_t nmemb, void *user_type_data){

	size_t realsize = size * nmemb;

	struct MemoryStruct* mem = ( struct MemoryStruct*)user_type_data;

	char* temporary_ptr = realloc(mem->memory, mem->size + realsize +1);

	if(temporary_ptr == NULL){

		snprintf(mem->buffer_error, BUFFER_SIZE, "%s", "Error, not enough memory for received transfer");

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

	char buffer_domain[BUFFER_SIZE];

	char buffer_error[BUFFER_SIZE];

	char buffer_token[BUFFER_SIZE];

	struct MemoryStruct user_data;

	user_data.memory = malloc(1);

	user_data.size = 0;

	memset(user_data.buffer_error, '\0',BUFFER_SIZE);

	CURL* handler = NULL;

	CURLcode res;

	curl_global_init(GLOBAL_INIT_BIT);

		handler = curl_easy_init();

			if(handler){

				curl_easy_setopt(handler, CURLOPT_URL, "https://example.com/");

				curl_easy_setopt(handler, CURLOPT_WRITEDATA, (void*)&user_data);

				curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, write_callback);

				res = curl_easy_perform(handler);

				if(res!=CURLE_OK){

					printf("Error calling curl_easy_perform: %s\n", curl_easy_strerror(res));

				}else{


					printf("The transfer was a success\n");

					printf("This is the content: \n %s", user_data.memory);


				}

			curl_easy_cleanup(handler);

			}else{
				printf("Error, curl_easy_init faild, returned null pointer\n");
			}

	curl_global_cleanup();


	return 0;
}
