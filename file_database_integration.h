

void __memory_table_to_file(char *database, TABLE *t)
{
    int row_count, column_count, status;
    char *data, *column_name, *database_path;
    int data_size = 100000;
    int column_name_size = 200;

    //Creating the database folder and table structure
    create_database(database);
    create_type(database, TABLE_TYPE, t->name);

    database_path = get_table_path(database, t->name);
    sprintf(logdata, "The table written to %s.\n", database_path);
    logger(logdata, CONSOLE);

    //Parsing the Data written to the file.
    for(column_count = 0; column_count < t->total_column; column_count++)
    {
        data = (char *)malloc(data_size);
        column_name = (char *)malloc(column_name_size);
        for(row_count = 0; row_count < t->total_rows; row_count++)
        {
            if(row_count == 0)
                sprintf(data, "%s", t->rows[row_count]->cell_data[column_count]);
            else
                sprintf(data, "%s\n%s", data, t->rows[row_count]->cell_data[column_count]);

        }
        sprintf(column_name, "%s/Data/column_%d", database_path, column_count);
        status = create_file(column_name, data);
        if(status == FAILURE)
        {
            sprintf(logdata, "Exiting the program because create_file function failed.");
            logger(logdata, ERROR);
            exit(FAILURE);
        }
        sprintf(logdata, "Modified for the column in file : %s\n", column_name);
        logger(logdata, DEBUG);
        bzero(data, data_size);
        free(column_name);
        free(data);
    }
    free(database_path);
}

void __create_config_file(char *database, void *tn, int type)
{
    char *database_path, *temp_data, *column_data;
    int status, column_count;
    long int name_size = 100000;
    TABLE *t;
    if(type == TABLE_TYPE)
    {
        t = (TABLE *)tn;
        database_path = get_table_path(database, t->name);
        sprintf(database_path, "%s/config.conf", database_path);
        temp_data = (char *)malloc(name_size);

        sprintf(temp_data, "TABLE %s", t->table_type);
        for(column_count = 0; column_count < t->total_column; column_count++)
        {
            sprintf(temp_data, "%s\ncolumn_%d\t%s\t%s", temp_data, column_count, t->c_type[column_count]->column_mode, t->c_type[column_count]->column_datatype);
        }
        column_data = (char *)malloc(strlen(temp_data)  + 1);
        strcpy(column_data, temp_data);
        free(temp_data);
        sprintf(logdata, "Writing below data to config_file.\n Data size: %ld bytes\n %s.\n", strlen(column_data), column_data);
        logger(logdata, DEBUG);
        status = create_file(database_path, column_data);
        if(status == FAILURE)
        {
            sprintf(logdata, "Exiting the program because create_file function failed.");
            logger(logdata, ERROR);
            exit(FAILURE);
        }
    }
    free(database_path);
    free(column_data);
}

void add_memory_to_file(char *database, void *t, int type)
{
    TABLE *tab;
    NODE *nod;

    if(type == TABLE_TYPE)
    {
        tab = (TABLE *)t;
        __memory_table_to_file(database, tab);
        __create_config_file(database, tab, type);
    }
}

int remove_database_table(char *database, void *t, int type)
{
    TABLE *tab;
    NODE *nod;
    int i;
    int total_column;
    char *col_name;

    if(type == TABLE_TYPE)
    {
        tab = (TABLE *)t;
        total_column = tab->total_column;

        //Delete all the column files and parent folder..
        for(i = 0; i < total_column; i++)
        {
            col_name = (char *)malloc(20);
            sprintf(col_name, "column_%d", i);
            if (delete_column(database, tab->name, col_name) == FAILURE)
                return FAILURE;
            free(col_name);
        }

        if(delete_table_files(database, tab->name) == FAILURE)
            return FAILURE;
        if(delete_table(database, tab->name) == FAILURE)
            return FAILURE;

        //Deintialize the table in the memory
        deinitialize_table(t);
    }
    return SUCCESS;
}

TABLE *reinitialize_table(char *database, TABLE *t)
{
    int row_count, column_count, total_columns = 0;
    ROW *row;
    char name[15];
    strcpy(name, t->name);

    deinitialize_table(t);

    char ***column_data = read_table_columns(database, name);

    while(1)
    {
        if(column_data[total_columns][0] == NULL) //Verify if the pointer reached the last column.
            break;
        total_columns++;
    }

    sprintf(logdata, "reinitialize_table: Read Columns and rows in the file");
    logger(logdata, DEBUG);
    t = (TABLE *)initialize_table(name, total_columns);
    char *data[total_columns];
    row_count = 0;
    while(1)
    {
        column_count = 0;
        if(column_data[0][row_count] == NULL) //Verify if the pointer reached the last row.
                break;
        while(column_count < total_columns)
        {
            if(column_data[column_count][row_count] == NULL) //Verify if the pointer reached the last column.
                break;
            data[column_count] = (char *)malloc(strlen(column_data[column_count][row_count]) + 1);
            sprintf(data[column_count], "%s", column_data[column_count][row_count]);
//            printf("%d:%d-> %s.\t", column_count, row_count, data[column_count]);
            column_count++;
        }
//        row = create_row(total_columns, data[0], data[1], data[2], data[3], data[4], data[5]);
        row = create_row(t->total_column, &*data);
        add_row_in_table(t, row);

//        printf("\n");
        row_count++;
    }

    column_count = 0;
    while(column_count <= total_columns)
    {
        row_count = 0;
        while(1)
        {
            if(column_data[column_count][row_count] == NULL)
                break;
//            printf("%d:%d-> %s.\n", column_count, row_count, column_data[column_count][row_count]);
            free(column_data[column_count][row_count]);
            row_count++;
        }
        free(column_data[column_count][row_count]);
        free(column_data[column_count]);
        column_count++;
    }
    sprintf(logdata, "reinitialize_table: Completed reading Columns and rows in the file");
    logger(logdata, DEBUG);

    //Read from config file and parse the data into table format.
    sprintf(logdata, "reinitialize_table: Reading configuration file and parsed to table format.");
    logger(logdata, DEBUG);
    char **data_config = read_config_file(database, t->name);
    row_count = 0;
    column_count = 0;
    while(1)
    {
        if(data_config[row_count] == NULL)
        {
            free(data_config[row_count]);
            break;
        }

        if(strcmp(data_config[row_count], "TABLE") == 0)
        {
            strcpy(t->table_type, data_config[row_count + 1]);
            free(data_config[row_count]);
            free(data_config[row_count + 1]);
            row_count = row_count + 2;
            continue;
        }
        sprintf(name, "column_%d", column_count);
        if(strcmp(data_config[row_count], name)==0)
        {
            strcpy(t->c_type[column_count]->column_datatype, data_config[row_count + 2]);
            strcpy(t->c_type[column_count]->column_mode, data_config[row_count + 1]);
            free(data_config[row_count]);
            free(data_config[row_count + 1]);
            free(data_config[row_count + 2]);
            row_count = row_count + 3;
            column_count++;
            continue;
        }

        row_count++;
    }
    sprintf(logdata, "reinitialize_table: Completed reading configuration file and parsed to the table format.");
    logger(logdata, DEBUG);

    free(data_config);
    free(column_data);
    return t;
}

int remove_database_column(char *database, void *t, char *c_name)
{
    TABLE *tab;
    NODE *nod;
    int i, status = SUCCESS;
    int total_column;
    char *curr_col_name, *next_col_name;

    tab = (TABLE *)t;
    total_column = tab->total_column;

    //Delete all specific column files...
    for(i = 0; i < total_column; i++)
    {
        curr_col_name = (char *)malloc(20);
        sprintf(curr_col_name, "column_%d", i);

        if(strcmp(c_name, curr_col_name) == 0)
        {
            if (delete_column(database, tab->name, curr_col_name) == FAILURE)
            {
                free(curr_col_name);
                return FAILURE;
            }
            sprintf(logdata, "file_database_integration: Deleted column: %s", curr_col_name);
            logger(logdata, DEBUG);

            //Rename the subsequent files and column_types for later changes in the config files in same function.
            if(i>=total_column)
            {
                free(curr_col_name);
                break;
            }

            do
            {
                next_col_name = (char *)malloc(20);
                sprintf(next_col_name, "column_%d", i + 1);
                strcpy(tab->c_type[i]->column_datatype, tab->c_type[i + 1]->column_datatype);
                strcpy(tab->c_type[i]->column_mode, tab->c_type[i + 1]->column_mode);
                sprintf(logdata, "======> Renaming %s , %s. \n", next_col_name, curr_col_name);
                logger(logdata, DEBUG);
                status = rename_column_name(database, tab->name, next_col_name, curr_col_name);
                free(next_col_name);
                i++;
                sprintf(curr_col_name, "column_%d", i);
            }while(i<total_column - 1);
//            free(tab->c_type[i]);
            free(curr_col_name);
            break;
        }
        free(curr_col_name);
    }
    tab->total_column--;
    __create_config_file(database, (void *)tab, TABLE_TYPE);
    tab->total_column++;
    return status;
}
