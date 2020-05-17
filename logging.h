#define LOG_FILENAME "/var/AIDB/MemoryDaemon.log"

void logger(char *data, int mode)
{
    if(DEBUG_ENABLED == FALSE && (mode == DEBUG || mode == CONSOLE))
    {
            return;
    }

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    time_t t;
    char *ltime;
    FILE *fd;
    t = time(NULL);

    ltime = strtok(ctime(&t), "\n");
    fd = fopen(LOG_FILENAME, "a");

    char a[5];
    if(mode == INFO)
        strcpy(a, "INFO");
    else if(mode == ERROR)
        strcpy(a, "ERROR");
    else if(mode == DEBUG)
        strcpy(a, "DEBUG");
    else if(mode == CONSOLE)
    {

        printf("%s", data);
        fclose(fd);
        bzero((void *)data, sizeof(data));
        pthread_mutex_unlock(&lock);
        return;
    }
    else
    {
        strcpy(a, " ");
    }

    fprintf(fd, "%s :: %d :: %s :: %s\n", ltime, getpid(), a, data);
    fclose(fd);
    bzero((void *)data, sizeof(data));

    pthread_mutex_unlock(&lock);
}
