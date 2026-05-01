#include "image.h"

void render(SDL_Renderer* renderer, image_representation* image_file) {

    SDL_FRect destinationRect;

    destinationRect.x = image_file->m_start.m_column;

    destinationRect.y = image_file->m_start.m_row;

    destinationRect.h = image_file->m_dimensions.m_row;

    destinationRect.w = image_file->m_dimensions.m_column;

    SDL_RenderTexture(renderer, image_file->m_texture, NULL, &destinationRect);

}
void render_with_rotation(SDL_Renderer* renderer, image_representation* image_file){
//keep in mind that in image_file, we have already updated the angle of the image, which is stored inside of image_file already.
        SDL_FRect destinationRect = {0};

        destinationRect.x = image_file->m_start.m_column;

        destinationRect.y = image_file->m_start.m_row;

        destinationRect.h = image_file->m_dimensions.m_row;

        destinationRect.w = image_file->m_dimensions.m_column;

        //in SDL we need to use SDL_Fpoint
        SDL_FPoint temporaryPoint = {0};

        float center_m_row = image_file->m_dimensions.m_row / 2 + image_file->m_start.m_row;

        float center_m_column = image_file->m_dimensions.m_column / 2 + image_file->m_start.m_column;


        temporaryPoint.x = center_m_column - image_file->m_start.m_column;
        temporaryPoint.y = center_m_row - image_file->m_start.m_row;
        SDL_RenderTextureRotated(renderer, image_file->m_texture, NULL, &destinationRect, (double)image_file->m_angle, &temporaryPoint, SDL_FLIP_NONE);
}
void destroyImageRepresentation(image_representation* image_file) {

    if (image_file != NULL) {

        if (image_file->m_texture != NULL) {

            SDL_DestroyTexture(image_file->m_texture);

            image_file->m_texture = NULL;

        }

        image_file->m_start.m_row = 0.0f;

        image_file->m_start.m_row = 0.0f;

        image_file->m_dimensions.m_row = 0.0f;

        image_file->m_dimensions.m_row = 0.0f;

        image_file->m_angle = 0.0f;

    }


}
