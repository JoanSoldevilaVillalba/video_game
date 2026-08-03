#pragma once


#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>


size_t read_all(int temporary_fd, char buffer[], size_t length);

size_t send_all(int temporary_fd, const char*  buffer, size_t length);
