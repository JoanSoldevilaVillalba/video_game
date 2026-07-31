#pragma once


#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>


int read_all(int temporary_fd, char buffer[], int length);

int send_all(int temporary_fd, const char*  buffer, int length);
