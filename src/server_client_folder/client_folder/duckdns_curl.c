#include <stdio.h>
#include "curl/curl.h"

#define BUFFER_SIZE 64

#if defined(_WIN32) || defined(_WIN64)
    #define GLOBAL_INIT_BIT CURL_GLOBAL_WIN32
#else
    #define GLOBAL_INIT_BIT CURL_GLOBAL_ALL
#endif

int main(void){

	char buffer_domain[BUFFER_SIZE];

	char buffer_error[BUFFER_SIZE];

	char buffer_token[BUFFER_SIZE];

	CURL* handler = NULL;

	curl_global_init(GLOBAL_INIT_BIT);

		handler = curl_easy_init();



		curl_easy_cleanup(handler);

	curl_global_cleanup();


	return 0;
}
