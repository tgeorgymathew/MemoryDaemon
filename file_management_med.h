
char *create_file(char *data)
{
    char *filename = (char *)malloc(FILENAME_SIZE);
    char *filepath = (char *)malloc(FILEPATH_SIZE);

    while(1)
    {
        __set_name(filename);
        sprintf(filepath, "%s%s", ROOT_STORAGE, filename);
        sprintf(logdata, "File name is '%s'", filepath);
        logger(logdata, DEBUG);
        if (access(filepath, F_OK) == 0)
        {
            sprintf(logdata, "The filepath '%s' already exist.", filepath);
            logger(logdata, DEBUG);
            sprintf(logdata, "Recreating the file name.");
            logger(logdata, DEBUG);
        }
        else
        {
            if (__write(filepath, data) == FAILURE)
            {
                free(filename);
                free(filepath);
                return FAILURE;
            }
            else
            {
                sprintf(logdata, "File name '%s' is added.", filepath);
                logger(logdata, INFO);
                break;
            }
        }
    }

    free(filepath);
    return filename;
}

int modify_file(char *filename, char *data)
{
    char *filepath = (char *)malloc(FILEPATH_SIZE);
    sprintf(filepath, "%s%s", ROOT_STORAGE, filename);
    sprintf(logdata, "File name is '%s'", filename);
    logger(logdata, DEBUG);
    if (__access_file_status(filepath, W_OK) != 0)
    {
        free(filepath);
        return FAILURE;
    }
    else
    {
        if (__write(filepath, data) == FAILURE)
        {
            free(filepath);
            return FAILURE;
        }
        else
        {
            sprintf(logdata, "File '%s' modified.", filepath);
            logger(logdata, INFO);
        }
    }

    free(filepath);
    return SUCCESS;
}

char *read_file(char *filename)
{
    int status;
    char *data = (char *)malloc(1000);
    char *filepath = (char *)malloc(FILEPATH_SIZE);

    sprintf(filepath, "%s%s", ROOT_STORAGE, filename);
    sprintf(logdata, "Reading file'%s'", filepath);
    logger(logdata, INFO);

    if (__access_file_status(filepath, R_OK) != 0)
    {
        return FAILURE;
    }

    status = __read(filepath, data);
    if (status == FAILURE)
        return FAILURE;
    return data;
}
