

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
//    bzero((void *)data, sizeof(data));
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


//void __set_name(char *name)
//{
//    long int rand = random();
//    char *data = (char *)malloc(10);
//    sprintf(name, "%p-%ld", data, rand);
//    free(data);
//}

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


int create_folder(char *folder_name)
{
    sprintf(logdata, "Creating the folder '%s'.", folder_name);
    logger(logdata, DEBUG);
    int status = mkdir(folder_name, 0777);

    if(status == -1)
    {
        sprintf(logdata, "The folder '%s' could not be created. Reason: %s", folder_name, strerror(errno));
        logger(logdata, DEBUG);
        return FAILURE;
    }

    sprintf(logdata, "The folder '%s' created successfully.", folder_name);
    logger(logdata, DEBUG);
    return SUCCESS;
}

int create_database(char *name)
{
    char *folder_name = (char *)malloc(sizeof(ROOT_STORAGE) + 15);

    //Creating Database folder
    sprintf(folder_name, "%s%s", ROOT_STORAGE, name);
    int status = create_folder(folder_name);

    //Creating the table type in the Data folder
    char *type_path = (char *)malloc(strlen(folder_name) + 8);
    sprintf(type_path, "%s/TABLE", folder_name);
    status = create_folder(type_path);

    //Creating the node type in the Data folder
    sprintf(type_path, "%s/Node", folder_name);
    status = create_folder(type_path);

    free(type_path);
    free(folder_name);
    return status;
}

int create_type(char *database, int type, char *name)
{
    char *folder_name = (char *)malloc(strlen(ROOT_STORAGE) + 70);
    if(type == TABLE_TYPE)
        sprintf(folder_name, "%s%s/TABLE/%s", ROOT_STORAGE, database, name);
    else if(type == NODE_TYPE)
        sprintf(folder_name, "%s%s/NODE/%s", ROOT_STORAGE, database, name);
    int status = create_folder(folder_name);
    sprintf(folder_name, "%s/Data", folder_name);
    status = create_folder(folder_name);

    free(folder_name);
    return status;
}

int create_file(char *Database_path, char *data)
{
//    char *filename = (char *)malloc(FILENAME_SIZE);
    char *filepath = (char *)malloc(strlen(ROOT_STORAGE) + strlen(Database_path) + 1);

//    while(1)
//    {
//        __set_name(filename);
    sprintf(filepath, "%s%s", ROOT_STORAGE, Database_path);
    sprintf(logdata, "File name is '%s'", filepath);
    logger(logdata, DEBUG);
    if (__access_file_status(filepath, F_OK) == 0)
    {
        sprintf(logdata, "The filepath '%s' already exist.", filepath);
        logger(logdata, DEBUG);
        sprintf(logdata, "Recreating the file name.");
        logger(logdata, DEBUG);
    }
//    else
//    {

    if (__write(filepath, data) == FAILURE)
    {
//        free(filename);
        free(filepath);
        return FAILURE;
    }
    else
    {
        sprintf(logdata, "File name '%s' is added.", filepath);
        logger(logdata, INFO);
//        break;
    }
//    }
//    }

    free(filepath);
    return SUCCESS;
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
