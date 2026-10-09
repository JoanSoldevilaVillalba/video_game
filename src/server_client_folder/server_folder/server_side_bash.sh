#!/bin/bash

gcc server_main.c server_logic.c server_recv_data.c server_send_data.c server_error_network_handler.c  -pthread -I/home/goldenboy/projectes/video_game/include/server -o server.out


