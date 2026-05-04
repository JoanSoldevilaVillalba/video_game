#include "loop_logic.h"

void rendering(SDL_Renderer* renderer, image_representation* default_player){

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderClear(renderer);

        render_with_rotation(renderer, default_player);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        float center_m_row = default_player->m_dimensions.m_row/2 + default_player->m_start.m_row;

        float center_m_column = default_player->m_dimensions.m_column/2 + default_player->m_start.m_column;

        SDL_RenderPoint(renderer, center_m_column, center_m_row);

        SDL_RenderPresent(renderer);

}

float calculateAngleQuadrent(float mouseX, float mouseY, float row_user, float column_user){

    float angle_final = 0.0f;

    float m_dif_x = (mouseX - column_user);

    float m_dif_y = (mouseY - row_user);

    float distance_between_points = sqrtf(m_dif_x * m_dif_x + m_dif_y * m_dif_y);

    angle_final = atan2f(m_dif_y,m_dif_x);

    angle_final = angle_final * (180.0f / M_PI);

    if(angle_final<0){

        angle_final+=360.0f;

    }

    return angle_final;

}

bool eventHandler(SDL_Event* event,float* mouseX, float* mouseY, image_representation* default_player){
        bool quit = false;
        bool movement = false;
        if (event->type == SDL_EVENT_QUIT) {


        return true;
            }
        if (event->type == SDL_EVENT_MOUSE_MOTION) {
                        *(mouseX) = event->motion.x;
                        *(mouseY) = event->motion.y;
                        movement = true;

                //in here we are going to have to check if  we are clicking the mouse, because now we are going to do the animations for punching
                }else if (event->type == SDL_EVENT_KEY_DOWN) {

                        switch(event->key.key){

                                case SDLK_UP:

                                        default_player->m_start.m_row -=1;

                                        movement = true;

                                        break;

                                case SDLK_DOWN:

                                        default_player->m_start.m_row +=1;

                                        movement = true;

                                        break;
                                case SDLK_LEFT:

                                        default_player->m_start.m_column -=1;

                                        movement = true;

                                        break;

                                case SDLK_RIGHT:
                                        default_player->m_start.m_column +=1;

                                        movement = true;

                                        break;
                                default:

                                        SDL_Log("Error, you have pressed a button that is not yet allowed");

                                        break;
                        }

                    }

        if(movement){

                float center_m_row = (default_player->m_dimensions.m_row/2.0f) + default_player->m_start.m_row;

                float center_m_column = (default_player->m_dimensions.m_column/2.0f) + default_player->m_start.m_column;

                default_player->m_angle = calculateAngleQuadrent(*(mouseX), *(mouseY), center_m_row,center_m_column);

        }


        return quit;

}
