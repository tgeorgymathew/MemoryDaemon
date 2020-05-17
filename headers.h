#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#define MAX_COLUMNS 1000
#define MAX_ROWS 100000
#define MAX_CELL_SIZE 1000
#define YES 1
#define NO 0
#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define FAILURE 0
#define LOGSIZE 50000
#define KEEPALIVE_TIMEOUT 10
#define SOCKET_TIMEOUT_RECV 10
#define SOCKET_TIMEOUT_SEND 10
#define FILENAME_SIZE 20
#define FILEPATH_SIZE 50
#define ROOT_STORAGE "/var/AIDB/"
#define READ "rb"
#define WRITE "wb+"
#define CONSOLE 0
#define INFO 1
#define ERROR 2
#define DEBUG 3
#define DEBUG_ENABLED TRUE
#define TABLE_TYPE 1
#define NODE_TYPE 2
#define DEFAULT 0
#define MEMORY 1
/*Node Configuration*/
#define ROOT_NODE 0
#define BRANCH_NODE 1

char *logdata;

//Builtin Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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
//#include "SHA384_512.h"


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#include "logging.h"

//Custom Header files(Low Level)
#include "Misc.h"
#include "file_management_low.h"
#include "database_format_low.h"

//Custom Header files(Med Level)
#include "file_database_integration.h"

//Custom header files(High Level)
#include "database_structure.h"


#endif // HEADERS_H_INCLUDED
