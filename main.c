#include "headers.h"

void main()
{
    char *log = (char *) malloc(LOGSIZE);
    strcpy(log , "INFO: Memory process started....\n");
    logger(log);
    strcpy(log , "INFO: Memory process ended -1...\n");
    logger(log);
    strcpy(log , "INFO: Memory process ended -final...\n");
    logger(log);
}
