#define PATH_EXTRA_SIZE 50
#define NODE_CONTENT_SIZE 1000
#define DELIMITOR "|@#|"

char *get_database_path(char *database)
{
    char *path = (char *)malloc(strlen(database) + PATH_EXTRA_SIZE);
    sprintf(path, "%s%s", ROOT_STORAGE, database);
    return path;
}

char *get_table_path(char *database, char *table_name)
{
    char *path = (char *)malloc(strlen(database) + strlen(table_name) + PATH_EXTRA_SIZE);

    sprintf(path, "%s%s/TABLE/%s", ROOT_STORAGE, database, table_name);
    return path;
}

char *get_node_path(char *database, char *node_name)
{
    char *path = (char *)malloc(strlen(database) + strlen(node_name) + PATH_EXTRA_SIZE);

    sprintf(path, "%s%s/NODE/%s", ROOT_STORAGE, database, node_name);
    return path;
}

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
    return SUCCESS;
}

int __append(char *filepath, char *data)
{
    FILE *fd;
    fd = fopen(filepath, APPEND);
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
    return SUCCESS;
}

int __access_file_status(char *filepath, int mode)
{
    char *logdata = (char *)malloc(LOGSIZE);
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
    free(logdata);
    return status;
}


int create_folder(char *folder_name)
{
    char *logdata = (char *)malloc(LOGSIZE);
    sprintf(logdata, "Creating the folder '%s'.", folder_name);
    logger(logdata, DEBUG);
    int status = mkdir(folder_name, 0777);

    if(status == -1)
    {
        if(errno != 17)
        {
            sprintf(logdata, "The folder '%s' could not be created. Reason: %s", folder_name, strerror(errno));
            logger(logdata, ERROR);
            free(logdata);
            return FAILURE;
        }
        else
        {
            sprintf(logdata, "The folder '%s' could not be created. Reason: %s. Ignore and proceed.", folder_name, strerror(errno));
            logger(logdata, DEBUG);
        }

    }

    sprintf(logdata, "The folder '%s' created successfully.", folder_name);
    logger(logdata, DEBUG);
    free(logdata);
    return SUCCESS;
}

int create_database(char *name)
{
    //Creating Database folder
    char *folder_name = get_database_path(name);
    int status = create_folder(folder_name);

    //Creating the table type in the Data folder
    char *type_path = (char *)malloc(strlen(folder_name) + PATH_EXTRA_SIZE);
    sprintf(type_path, "%s/TABLE", folder_name);
    status = create_folder(type_path);

    //Creating the node type in the Data folder
    sprintf(type_path, "%s/NODE", folder_name);
    status = create_folder(type_path);

    sprintf(type_path, "%s/executables", folder_name);
    status = create_folder(type_path);

    sprintf(type_path, "%s/tmp", folder_name);
    status = create_folder(type_path);

    free(type_path);
    free(folder_name);
    return status;
}

int create_type(char *database, int type, char *name)
{
    char *folder_name;
    if(type == TABLE_TYPE)
        folder_name = get_table_path(database, name);
    else if(type == NODE_TYPE)
        folder_name = get_node_path(database, name);
    else
        return FAILURE;
    int status = create_folder(folder_name);
    sprintf(folder_name, "%s/Data", folder_name);
    status = create_folder(folder_name);

    free(folder_name);
    return status;
}

int create_file(char *Database_path, char *data)
{
    char *logdata = (char *)malloc(LOGSIZE);
    char *filepath = Database_path;
    sprintf(logdata, "File name is '%s'", filepath);
    logger(logdata, DEBUG);
    if (__access_file_status(filepath, F_OK) == 0)
    {
        sprintf(logdata, "The filepath '%s' already exist.", filepath);
        logger(logdata, DEBUG);
        sprintf(logdata, "Recreating the file name.");
        logger(logdata, DEBUG);
    }

    if (__write(filepath, data) == FAILURE)
    {
        free(filepath);
        free(logdata);
        return FAILURE;
    }
    else
    {
        sprintf(logdata, "File name '%s' is added.", filepath);
        logger(logdata, DEBUG);
    }
    free(logdata);
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
            logger(logdata, DEBUG);
        }
    }

    free(filepath);
    return SUCCESS;
}

int append_file(char *filename, char *data)
{
    char *logdata = (char *)malloc(LOGSIZE);
    sprintf(logdata, "File name is '%s'", filename);
    logger(logdata, DEBUG);
    if (__access_file_status(filename, R_OK) != 0)
    {
//        free(filepath);
        free(logdata);
        return FAILURE;
    }

    if(__append(filename, data) ==FAILURE)
    {
//        free(filepath);
        free(logdata);
        return FAILURE;
    }
    else
    {
        sprintf(logdata, "File '%s' appended.", filename);
        logger(logdata, DEBUG);
    }
//    free(filepath);
    free(logdata);
    return SUCCESS;
}

int delete_column(char *database, char *table_name, char *col_name)
{
    char *col_path = get_table_path(database, table_name);
    sprintf(col_path, "%s/Data/%s", col_path, col_name);
    remove(col_path);

    int status = __access_file_status(col_path, F_OK);
    free(col_path);
    if(status != -1)
        return FAILURE;
    return SUCCESS;
}

int delete_table_files(char *database, char *table_name)
{
    char *col_path = get_table_path(database, table_name);
    sprintf(col_path, "%s/config.conf", col_path);
    remove(col_path);

    int status = __access_file_status(col_path, F_OK);
    free(col_path);
    if(status != -1)
        return FAILURE;
    return SUCCESS;
}

int delete_all_column_files(char *database, char *table_name)
{
    char *col_path = get_table_path(database, table_name);
    sprintf(col_path, "%s/Data/*", col_path);

    char *command = (char *)malloc(strlen(col_path) + 15);
    sprintf(command, "sudo rm -rf %s", col_path);
    int status = system(command);
    if(status == 0)
        status = SUCCESS;
    else
        status = FAILURE;
    free(command);
    free(col_path);
    return status;
}

int delete_table(char *database, char *table_name)
{
    //Delete the data folder.
    char *path = get_table_path(database, table_name);
    sprintf(path, "%s/Data", path);
    int status = rmdir(path);
    free(path);
    if(status == -1)
    {
        sprintf(logdata, "Couldn't delete the folder %s. Reason : %s.", path, strerror(errno));
        logger(logdata, DEBUG);
        return FAILURE;
    }

    //Delete the table folder.
    path = get_table_path(database, table_name);
    status = rmdir(path);
    free(path);
    if(status == -1)
    {
        sprintf(logdata, "Couldn't delete the folder %s. Reason : %s.", path, strerror(errno));
        logger(logdata, DEBUG);
        return FAILURE;
    }

    return SUCCESS;
}

int rename_column_name(char *database, char *table_name, char *old_column, char *new_column)
{
    char *table_path = get_table_path(database, table_name);

    char *old_path, *new_path;
    old_path = (char *)malloc(strlen(table_path) + FILENAME_SIZE);
    new_path = (char *)malloc(strlen(table_path) + FILENAME_SIZE);

    sprintf(old_path, "%s/Data/%s", table_path, old_column);
    sprintf(new_path, "%s/Data/%s", table_path, new_column);

    if(rename(old_path, new_path) < 0)
    {
        printf("Error occurred: '%s'->'%s'. Reason: %s.\n", old_path, new_path, strerror(errno));
        free(table_path);
        return FAILURE;
    }

    free(old_path);
    free(new_path);
    free(table_path);
    return SUCCESS;
}

int __read(char *filename, char *data)
{
    FILE *fd;
    char *logdata = (char *)malloc(LOGSIZE);
    fd = fopen(filename, READ);
//    char *temp;

    if(fd==NULL)
    {
        sprintf(logdata, "Unable to open file, hence file could not be read. Path: %s", filename);
        logger(logdata, ERROR);
        free(logdata);
        return FAILURE;
    }

    int i = 0;
    while(1)
    {
        if(fscanf(fd, "%s", data+i) == EOF)
        {
            sprintf(logdata, "Read from file Path: %s finished.", filename);
            logger(logdata, DEBUG);
            break;
//            return FAILURE;
        }
        sprintf(data, "%s%s", data, DELIMITOR);
        i = strlen(data);
    }
    sprintf(data, "%sEOF", data);
//    data = temp;
    fclose(fd);
    free(logdata);
    return SUCCESS;
}

char *__read2(char *filename)
{
    FILE *fd;
    char *logdata = (char *)malloc(LOGSIZE);
    char *data, *temp_data;
    fd = fopen(filename, READ);
//    char *temp;

    if(fd==NULL)
    {
        sprintf(logdata, "Unable to open file, hence file could not be read. Path: %s", filename);
        logger(logdata, ERROR);
        free(logdata);
        return FAILURE;
    }

    temp_data = (char *)calloc(NODE_CONTENT_SIZE, 1);
    int status = fread((void *)temp_data, 1, NODE_CONTENT_SIZE, fd);

    if(status == 0)
    {
        fclose(fd);
        free(temp_data);
        free(logdata);
        return FAILURE;
    }
    data = (char *)malloc(strlen(temp_data) + 1);
    strcpy(data, temp_data);
//    sprintf(data, "%s%sEOF", data, DELIMITOR);
    free(temp_data);
    fclose(fd);
    free(logdata);
    return data;
}

char *read_file(char *filename)
{
    int status;
    char *logdata = (char *)malloc(LOGSIZE);
    sprintf(logdata, "Reading file'%s'", filename);
    logger(logdata, DEBUG);

    if (__access_file_status(filename, R_OK) != 0)
    {
        free(logdata);
        return FAILURE;
    }

    char *temp_data = (char *)malloc(MAX_CELL_SIZE * MAX_ROWS);
    status = __read(filename, temp_data);
    if (status == FAILURE)
    {
        free(logdata);
        free(temp_data);
        return FAILURE;
    }
    char *data = (char *)malloc(strlen(temp_data)  + 1);
    strcpy(data, temp_data);
    free(logdata);
    free(temp_data);
    return data;
}

char ***read_table_columns(char *database, char *table_name)
{
    char *table_path = get_table_path(database, table_name);
    char *columns_unformat, *temp_unformat, ***columns_format, *temp;
    char *filename = (char *)malloc(strlen(table_path) + 20);
    int j = 0;
    int i, total_columns = 1;
    int total_rows = 1;

    //Finding number columns in the file for a table.
    while(1)
    {
        sprintf(filename, "%s/Data/column_%d", table_path, j);
        columns_unformat = read_file(filename);
        if(columns_unformat == FAILURE)
        {
            j++;
            free(columns_unformat);
            break;
        }
        free(columns_unformat);
        j++;
    }
    total_columns = j;

    //Find number rows in the column_0 file.
    sprintf(filename, "%s/Data/column_0", table_path);
    columns_unformat = read_file(filename);
    temp_unformat = columns_unformat;
    while(1)
    {
        temp = (char *)malloc(100);
        sscanf(columns_unformat, "%[^|@#|]s", temp);
        if(strcmp(temp, "EOF")==0)
        {
            free(temp);
            break;
        }
        columns_unformat = columns_unformat + strlen(temp) + strlen(DELIMITOR);
        free(temp);
        total_rows++;
    }
    columns_unformat = temp_unformat;
    free(columns_unformat);


//    printf("Total Column is %d.\n", total_columns);
//    printf("Total row is %d.\n", total_rows);

    //Parse the data in file into rows and columns.
    columns_format = (char ***)malloc(sizeof(char *) * total_columns); //Malloc new pointers to hold row and columns.

    //Parse columns in each file.
    j = 0;
    while(j < total_columns)
    {
        sprintf(filename, "%s/Data/column_%d", table_path, j);
        columns_unformat = read_file(filename);
        if(columns_unformat == FAILURE)
            break;

        temp_unformat = columns_unformat;
        columns_format[j] = (char **)malloc(sizeof(char *) * total_rows); //Malloc pointers for column which contains rows..

        //Parse rows for each columns
        i = 0;
        while(i < total_rows)
        {
            temp = (char *)malloc(100);
            sscanf(columns_unformat, "%[^|@#|]s", temp);
            if(strcmp(temp, "EOF")==0)
            {
                free(temp);
                break;
            }
            columns_format[j][i] = (char *)malloc(strlen(temp) + 1); //Malloc pointer for rows.
            strcpy(columns_format[j][i], temp);
            free(temp);
            columns_unformat = columns_unformat + strlen(columns_format[j][i]) + strlen(DELIMITOR);
            i++;
        }
        columns_format[j][i] = '\0';
        bzero((void *)filename, strlen(filename) + 1);
        columns_unformat = temp_unformat;
        free(columns_unformat);
        sprintf(logdata, "Number of rows parsed is %d in column %d.", i, j);
        logger(logdata, DEBUG);
        j++;
    }

    //Append NULL character to all the rows and columns. It help to determine end of rows and columns..
    columns_format[j] = (char **)malloc(sizeof(char *) * i);
    while(i > 0)
    {
        columns_format[j][i - 1] = '\0';
        i--;
    }

    free(table_path);
    free(filename);
    return columns_format;
}


char **read_config_file(char *database, char *table_name)
{
    char *table_path = get_table_path(database, table_name);
    char *filename, *temp, *data_unformated, *temp_unformated, **data;
    int i = 0;

    filename = (char *)malloc(strlen(table_path) + FILENAME_SIZE);
    sprintf(filename, "%s/config.conf", table_path);

    data_unformated = read_file(filename);
    temp_unformated = data_unformated;
    while(1)
    {
        temp = (char *)malloc(100);
        sscanf(data_unformated, "%[^|@#|]s", temp);
        if(strcmp(temp, "EOF") == 0)
        {
            free(temp);
            break;
        }
        data_unformated = data_unformated + strlen(temp) + strlen(DELIMITOR);
        free(temp);
        i++;
    }
    data_unformated = temp_unformated;

    data = (char **)malloc(sizeof(char *) * (i + 1));
    i = 0;
    temp_unformated = data_unformated;
    while(1)
    {
        temp = (char *)malloc(100);
        sscanf(data_unformated, "%[^|@#|]s", temp);
        if(strcmp(temp, "EOF") == 0)
        {
            free(temp);
            break;
        }
        data[i] = temp;
//        printf("%s. \n", data[i]);
        data_unformated = data_unformated + strlen(temp) + strlen(DELIMITOR);
        i++;
    }
    data_unformated = temp_unformated;
    data[i] = '\0';

    free(filename);
    free(data_unformated);
    free(table_path);
    return data;
}

/*=================================NODE======================================*/

int delete_node_folder(char *database_name)
{
    int status;
    char *logdata = (char *)malloc(LOGSIZE);
    char *command = (char *)malloc(FILENAME_MAX);

    sprintf(command, "sudo rm -rf %s%s/NODE/*", ROOT_STORAGE, database_name);
    sprintf(logdata, "Deleting the contents in folder: %s%s/NODE/. Executing command: %s", ROOT_STORAGE, database_name, command);
    logger(logdata, DEBUG);
    status = system(command);
    if(status < 0)
    {
        sprintf(logdata, "Deleting the contents in folder failed");
        logger(logdata, DEBUG);
        free(logdata);
        free(command);
        return FAILURE;
    }
    free(logdata);
    free(command);
    return SUCCESS;
}

int node_delete_file(char *database, long int name)
{
    char *n = (char *)malloc(sizeof(long int));
    char *logdata = (char *)malloc(LOGSIZE);
    char *command = (char *)malloc(FILENAME_MAX);

    sprintf(n, "%ld", name);
    char *filepath = get_node_path(database, n);
    sprintf(command, "sudo rm -rf %s", filepath);
    sprintf(logdata, "Deleting the contents in folder: %s. Executing command: %s", filepath, command);
    logger(logdata, DEBUG);

    int status = system(command);
    if(status < 0)
    {
        sprintf(logdata, "Deleting the contents in folder failed");
        logger(logdata, DEBUG);
        free(filepath);
        free(logdata);
        free(command);
        return FAILURE;
    }

    free(filepath);
    free(logdata);
    free(command);
    return SUCCESS;
}


char **__node_read_file(char *database, char *file_path)
{
//    char *folder_path = get_node_path(database, filename);
//    char *file_path = (char *)malloc(strlen(folder_path) + PATH_EXTRA_SIZE);
    char *temp, *temp_data_unformatted;

//    /var/AIDB/Georgy_DB/NODE/1/Data/content
//    sprintf(file_path, "%s/Data/content", folder_path);

    char *data_unformatted = read_file(file_path);
    if(data_unformatted == FAILURE)
        return FAILURE;
    temp_data_unformatted = data_unformatted;

    //Find number of arrays to be created.
    int i = 0;
    while(1)
    {
        temp = (char *)malloc(strlen(data_unformatted) + 1);
        sscanf(data_unformatted, "%[^|@#|]s", temp);
        if(strcmp(temp, "EOF") == 0)
        {
            free(temp);
            break;
        }
        data_unformatted = data_unformatted + strlen(temp) + strlen(DELIMITOR);
        i++;
        free(temp);
    }
    data_unformatted = temp_data_unformatted;

    //Assign values to each array.
    int total_count = i;
    char **data = (char **)malloc(sizeof(char *) * (total_count + 1));
    i = 0;
    while(1)
    {
        temp = (char *)malloc(strlen(data_unformatted)+1);
        sscanf(data_unformatted, "%[^|@#|]s", temp);
        if(strcmp(temp, "EOF") == 0)
        {
            free(temp);
            break;
        }
        data[i] = (char *)malloc(strlen(temp) + 1);
        sprintf(data[i], "%s", temp);
        data_unformatted = data_unformatted + strlen(temp) + strlen(DELIMITOR);
        i++;
        free(temp);
    }
    data_unformatted = temp_data_unformatted;
    data[i] = '\0';

    free(temp_data_unformatted);
//    free(folder_path);

    return data;
}

char *__node_content_read_file(char *filepath)
{
//    char *temp_data = (char *)malloc(NODE_CONTENT_SIZE);
    char *temp_data = __read2(filepath);
    return temp_data;
}

char *node_content_read_file(char *database, char *filename)
{
    char *folder_path = get_node_path(database, filename);
    char *file_path = (char *)malloc(strlen(folder_path) + PATH_EXTRA_SIZE);

//    /var/AIDB/Georgy_DB/NODE/1/Data/content
    sprintf(file_path, "%s/Data/content", folder_path);
    free(folder_path);

//    char **data = __node_read_file(database, file_path);
//    if(data == FAILURE)
//    {
//        free(data);
//        free(file_path);
//        return FAILURE;
//    }
//    char *data_ret = (char *)malloc(strlen(data[0]) + 1);
//    strcpy(data_ret, data[0]);

    char *data_ret = __node_content_read_file(file_path);

//    free(data[0]);
//    free(data);
    free(file_path);
    return data_ret;
}

char **node_config_read_file(char *database, char *filename)
{
    char *folder_path = get_node_path(database, filename);
    char *file_path = (char *)malloc(strlen(folder_path) + PATH_EXTRA_SIZE);

//    /var/AIDB/Georgy_DB/NODE/1/Data/content
    sprintf(file_path, "%s/node.config", folder_path);
    free(folder_path);

    char **data = __node_read_file(database, file_path);
    if(data == FAILURE)
    {
        free(file_path);
        return FAILURE;
    }
    free(file_path);
    return data;
}
/*===================================NODE COMPLETED====================================*/
