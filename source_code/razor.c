#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

const int screenWidth = 640;
const int screenHeight = 480;

typedef struct{


	int m_row;

	int m_column;


}image_point;

typedef struct{


	image_point m_center;

	image_point m_start;

	image_point m_dimensions;

	float m_angle;

	SDL_Texture* m_texture;


}image_representation;


int loadFromFile(const char* image_path, image_representation* image_file, SDL_Renderer* renderer){

	int exit_code = 0;

	SDL_Surface* temporarySurface = IMG_Load(image_path);

	if(temporarySurface == NULL){

		SDL_Log("Error, we were not able to load a surface %s\n", SDL_GetError());

		exit_code = 1;

	}else{

		image_file->m_texture = SDL_CreateTextureFromSurface(renderer, temporarySurface);

		if(image_file->m_texture == NULL){

			SDL_Log("Error, we were not able to create a texture from the surface");

			exit_code = 1;
		}else{

			image_file->m_dimensions.m_row = temporarySurface->h;

			image_file->m_dimensions.m_column = temporarySurface->w;

		     }

	    }


	SDL_DestroySurface(temporarySurface);

	return exit_code;

}



int init(SDL_Window** window, SDL_Renderer** renderer){

	int exit_code = 0;

	if(SDL_Init(SDL_INIT_VIDEO)){

		if(!SDL_CreateWindowAndRenderer("SDL3 tutorial: this is just for practice",screenWidth, screenHeight,0,window, renderer)){
			exit_code = 1;
		}

	}else{

		exit_code = 1;
	     }

	return exit_code;

}


int loadMedia(image_representation* image_file, const char* image_path,SDL_Renderer* renderer){

	int exit_code = 0;

	exit_code = loadFromFile(image_path, image_file, renderer);

	return exit_code;

}


void render(float x, float y, SDL_Renderer* renderer, image_representation*  image_file){

	SDL_FRect destinationRect;

	destinationRect.x = x;

	destinationRect.y = y;

	destinationRect.h = image_file->m_dimensions.m_row;

	destinationRect.w = image_file->m_dimensions.m_column;

	SDL_RenderTexture(renderer, image_file->m_texture,NULL, &destinationRect);

}

void destroyImageRepresentation(image_representation* image_file){

	if(image_file !=NULL){

		if(image_file->m_texture!=NULL){

			SDL_DestroyTexture(image_file->m_texture);

		}

	image_file->m_texture=NULL;

	image_file->m_center.m_row = 0;

	image_file->m_center.m_column = 0;

	image_file->m_start.m_row = 0;

	image_file->m_start.m_row = 0;

	image_file->m_dimensions.m_row = 0;

	image_file->m_dimensions.m_row = 0;

	image_file->m_angle = 0;

	}


}



void closeRendererWindow(SDL_Renderer** renderer, SDL_Window** window){

	if(*(renderer)!=NULL){

		SDL_DestroyRenderer(*(renderer));

		*(renderer)=NULL;

	}

	if(*(window)!=NULL){

		SDL_DestroyWindow(*(window));

		*(window) = NULL;

	}

}


int main(int argc,char* argv[]){

	SDL_Window* window = NULL;

	SDL_Renderer* renderer = NULL;

	image_representation practice_png = {0};

	int exit_code = 0;

	image_representation array_practice_png[8];
	const char* practice_punches[8]={"../videoGameImatges/punch_left/first.png","../videoGameImatges/punch_left/second.png","../videoGameImatges/punch_left/third.png","../videoGameImatges/punch_left/fourth.png","../videoGameImatges/punch_left/fith.png","../videoGameImatges/punch_left/sixth.png", "../videoGameImatges/punch_left/seventh.png", "../videoGameImatges/punch_left/vuit.png"};

	exit_code = init(&window,&renderer);

	if(exit_code !=0){

		SDL_Log("error");

		return exit_code;

	}


	for(int i =0;i<8;i++){

		exit_code = loadMedia(&array_practice_png[i],practice_punches[i],renderer);
		if(exit_code!=0){
			break;
		}
	}

	if(exit_code!=0){

		SDL_Log("error");

		return exit_code;

	}


	bool quit = false;

	SDL_Event event;

	SDL_zero(event);

	image_representation* current_image = NULL;

	int counter_image = 0;

	current_image = &array_practice_png[0];

	while(!quit){

		while(SDL_PollEvent(&event)){

			if(event.type == SDL_EVENT_QUIT){
				quit = true;
			}else if(event.type == SDL_EVENT_KEY_DOWN){

				if(event.key.key == SDLK_UP){
					current_image=&array_practice_png[counter_image++];
					if(counter_image >= 8){
						counter_image = 0;
					}
				}
			}else if(event.type == SDL_EVENT_QUIT){
				printf("hem acabat\n");
				return 9;
			}
		}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_RenderClear(renderer);

	render(0.f,0.f,renderer, current_image);

	SDL_RenderPresent(renderer);


	}


//	destroyImageRepresentation(&practice_png);

	closeRendererWindow(&renderer, &window);

	return 0;
}



