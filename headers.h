#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#define INITIAL 99
#define YES 1
#define NO 0
#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define FAILURE 0
#define MODIFIED 2
#define BUSY 2
#define LOGSIZE 1000
#define KEEPALIVE_TIMEOUT 10
#define SOCKET_TIMEOUT_RECV 10
#define SOCKET_TIMEOUT_SEND 10
char *logdata;


//Builtin Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

pthread_mutex_t lock;

//Custom Header files(Low Level)


//Custom Header files(Med Level
#include "logging.h"

#endif // HEADERS_H_INCLUDED
