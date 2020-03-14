

int __write(char *filepath, char *data)
{
    FILE *fd;
    fd = fopen(filepath, WRITE);
    if(fd==NULL)
    {
        sprintf(logdata, "Unable to open file, hence data cannot be written to the file. Path: %s", filepath);
        logger(logdata, ERROR);
        return FAILURE;
    }
    if(fprintf(fd, "%s", data) < 0)
    {
        sprintf(logdata, "Unable to write to file. Path: %s", filepath);
        logger(logdata, ERROR);
        return FAILURE;
    }
    fclose(fd);
    bzero((void *)data, sizeof(data));
    return SUCCESS;
}

int __read(char *filename, char *data)
{
    FILE *fd;
    fd = fopen(filename, READ);
    if(fd==NULL)
    {
        sprintf(logdata, "Unable to open file, hence file could not be read. Path: %s", filename);
        logger(logdata, ERROR);
        return FAILURE;
    }

    if(fscanf(fd, "%s", data) < 0)
    {
        sprintf(logdata, "Unable to read from file. Path: %s", filename);
        logger(logdata, ERROR);
        return FAILURE;
    }
    fclose(fd);
    return SUCCESS;
}


void __set_name(char *name)
{
    long int rand = random();
    char *data = (char *)malloc(10);
    sprintf(name, "%p-%ld", data, rand);
    free(data);
}

int __access_file_status(char *filepath, int mode)
{
    int status = access(filepath, mode);
    if (mode == F_OK && status == 0)
    {
        sprintf(logdata, "The file '%s' already exist.", filepath);
        logger(logdata, DEBUG);
    }
    else if (mode == R_OK && status == 0)
    {
        sprintf(logdata, "The file '%s' can be read.", filepath);
        logger(logdata, DEBUG);
    }
    else if (mode == W_OK && status == 0)
    {
        sprintf(logdata, "The data can be written to file '%s'.", filepath);
        logger(logdata, DEBUG);
    }
    else if (mode == X_OK && status == 0)
    {
        sprintf(logdata, "The file '%s' is executable.", filepath);
        logger(logdata, DEBUG);
    }
    else
    {
        sprintf(logdata, "The file '%s' is not accessible.", filepath);
        logger(logdata, DEBUG);
    }
    return status;
}
