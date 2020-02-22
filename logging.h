#define LOG_FILENAME "/tmp/MemoryDaemon.log"

void logger(char *data)
{
    pthread_mutex_lock(&lock);
    time_t t;
    char *ltime;
    FILE *fd;
    t = time(NULL);

    ltime = strtok(ctime(&t), "\n");
    fd = fopen(LOG_FILENAME, "a");
    fprintf(fd, "%s :: %s\n", ltime, data);
    fclose(fd);
    bzero((void *)data, sizeof(data));

    pthread_mutex_unlock(&lock);
}
