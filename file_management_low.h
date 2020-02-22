#define ROOT_STORAGE "/var/AIDB/"

int __write(char *data, char *name)
{
    FILE *fd;
    char *root = (char *)malloc(sizeof(ROOT_STORAGE));
    char *logdata = (char *)malloc(LOGSIZE);
    strcpy(root, ROOT_STORAGE);
    fd = fopen(strcat(root, name), "w+");
    if(fd==NULL)
    {
        sprintf(logdata, "Unable to open file, hence data cannot be written to the file. Path: %s", root);
        logger(logdata);
        return FAILURE;
    }
    if(fprintf(fd, "%s", data) < 0)
    {
        sprintf(logdata, "Unable to write to file. Path: %s", root);
        logger(logdata);
        return FAILURE;
    }
    fclose(fd);
    bzero((void *)data, sizeof(data));
    free(root);
    return SUCCESS;
}

int __read(char *name, char *data)
{
    FILE *fd;
    char *root = (char *)malloc(sizeof(ROOT_STORAGE));
    strcpy(root, ROOT_STORAGE);
    fd = fopen(strcat(root, name), "r");
    if(fd==NULL)
    {
        sprintf(logdata, "Unable to open file, hence file could not be read. Path: %s", root);
        logger(logdata);
        return FAILURE;
    }
    if(fscanf(fd, "%s", data) < 0)
    {
        sprintf(logdata, "Unable to read from file. Path: %s", root);
        logger(logdata);
        return FAILURE;
    }
    fclose(fd);
    free(root);
    return SUCCESS;
}
