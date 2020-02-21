#include "headers.h"

void main()
{
    char *log = (char *) malloc(LOGSIZE);
    strcpy(log , "INFO: Memory process started....\n");
    logger(log);
    strcpy(log , "INFO: Memory process ended...\n");
    logger(log);
}
