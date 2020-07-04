
//Library to add memory data to the file.
int create_file_wth_struct_args(void *args_d)
{
    struct arguments
    {
        char *arg_1, *arg_2;
    };
    struct arguments *args = (struct arguments *)args_d;
    int status = create_file(args->arg_1, args->arg_2);
    free(args->arg_1);
    free(args->arg_2);
    free(args);
    return status;
}

//void __memory_table_to_file(char *database, TABLE *t)
//{
//    int row_count, column_count, status;
//    char *data, *column_name, *database_path;
//    int data_size = MAX_CELL_SIZE * t->total_rows;
//    int column_name_size = 200;
//    pid_t child_pid;
//
//    //Creating the database folder and table structure
//    create_database(database);
//    create_type(database, TABLE_TYPE, t->name);
//
//    database_path = get_table_path(database, t->name);
//    sprintf(logdata, "The table written to %s.\n", database_path);
//    logger(logdata, CONSOLE);

    //Parsing the Data that needs to be written to the file.
//    for(column_count = 0; column_count < t->total_column; column_count++)
//    {
//        data = (char *)malloc(data_size);
//        column_name = (char *)malloc(column_name_size);
//        for(row_count = 0; row_count < t->total_rows; row_count++)
//        {
//            if(row_count == 0)
//                sprintf(data, "%s", t->rows[row_count]->cell_data[column_count]);
//            else
//                sprintf(data, "%s\n%s", data, t->rows[row_count]->cell_data[column_count]);
//
//        }
//        sprintf(column_name, "%s/Data/column_%d", database_path, column_count);
//        child_pid = fork();
//        if(child_pid == 0)
//        {
//            status = create_file(column_name, data);
//            if(status == FAILURE)
//            {
//                sprintf(logdata, "Exiting the program because create_file function failed.");
//                logger(logdata, ERROR);
//                exit(FAILURE);
//            }
//            sprintf(logdata, "Modified for the column in file : %s\n", column_name);
//            logger(logdata, DEBUG);
//            exit(SUCCESS);
//        }
//        bzero(data, data_size);
//        free(column_name);
//        free(data);
//    }
//    wait(NULL);
//    free(database_path);
//}

int __memory_table_to_file(char *database, TABLE *t)
{
    int row_count, column_count, status;
    char *data, *column_name, *database_path;
    int data_size = MAX_CELL_SIZE * t->total_rows;
    int column_name_size = 200;
    pthread_t thread_id[t->total_column];
    pid_t child_pid;
    char *logdata = (char *)malloc(LOGSIZE);

    struct arguments
    {
        char *arg_1, *arg_2;
    };
    struct arguments *args;

    //Creating the database folder and table structure
    create_database(database);
    create_type(database, TABLE_TYPE, t->name);

    database_path = get_table_path(database, t->name);
    sprintf(logdata, "The table written to %s.\n", database_path);
    logger(logdata, CONSOLE);

    //Parsing the Data that needs to be written to the file.
    for(column_count = 0; column_count < t->total_column; column_count++)
    {
        data = (char *)malloc(data_size);
        column_name = (char *)malloc(column_name_size);
        args = (struct arguments *)malloc(sizeof(struct arguments));
        for(row_count = 0; row_count < t->total_rows; row_count++)
        {
            if(row_count == 0)
                sprintf(data, "%s", t->rows[row_count]->cell_data[column_count]);
            else
                sprintf(data, "%s\n%s", data, t->rows[row_count]->cell_data[column_count]);

        }
        sprintf(column_name, "%s/Data/column_%d", database_path, column_count);

        args->arg_1 = column_name;
        args->arg_2 = data;
        child_pid = fork();
        if(child_pid == 0)
        {
            pthread_create(&thread_id[column_count], NULL, (void *)create_file_wth_struct_args, (void *)args);
            pthread_join(thread_id[column_count], (void *)&status);
    //        status = create_file(column_name, data);
            if(status == FAILURE)
            {
                sprintf(logdata, "Exiting the program because create_file function failed. Reason: %s.", strerror(errno));
                logger(logdata, ERROR);
                free(logdata);
                return FAILURE;
            }
            sprintf(logdata, "Modified for the column in file : column_%d\n", column_count);
            logger(logdata, DEBUG);
            free(logdata);
            return SUCCESS;
        }
        else if(child_pid < 0)
        {
            sprintf(logdata, "Child process creation failed and hence closing the program.. Reason: %s.", strerror(errno));
            logger(logdata, ERROR);
            free(logdata);
            return FAILURE;
        }
        bzero(data, data_size);
        free(data);
        free(column_name);
        free(args);
    }

    while(waitpid(0, &status, 0) >= 0);
    free(database_path);

    free(logdata);
    return SUCCESS;
}

//Create table/node specific configuration file.
int __create_config_file(char *database, void *tn, int type)
{
    char *database_path, *temp_data, *column_data;
    int status, column_count;
    long int name_size = 100000;
    char *logdata = (char *)malloc(LOGSIZE);

    TABLE *t;

    //Create table configuration file.
    if(type == TABLE_TYPE)
    {
        t = (TABLE *)tn;
        database_path = get_table_path(database, t->name);
        sprintf(database_path, "%s/config.conf", database_path);
        temp_data = (char *)malloc(name_size);

        sprintf(temp_data, "TABLE %s", t->table_type);
        for(column_count = 0; column_count < t->total_column; column_count++)
        {
            sprintf(temp_data, "%s\ncolumn_%d\t%d\t%d", temp_data, column_count, t->c_type[column_count]->column_mode, t->c_type[column_count]->column_datatype);
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
            return FAILURE;
        }
    }
    free(database_path);
    free(column_data);
    free(logdata);
    return SUCCESS;
}

int __node_config_to_file(NODE *nod, char *node_path)
{
    int status;
    char *logdata = (char *)malloc(LOGSIZE);
    char *file_path = (char *)malloc(strlen(node_path) + PATH_EXTRA_SIZE);
    sprintf(file_path, "%s/node.config", node_path);
    char *data = (char *)malloc(1000);

    if(nod == nod->root_link)
        sprintf(data, "TYPE\t%d\nROOT\t%ld", nod->type, nod->root_link->name);
    else
        sprintf(data, "TYPE\t%d\nROOT\t%ld\nPREVIOUS\t%ld", nod->type, nod->root_link->name, nod->previous_link->name);
    int i;
    for(i=0; i<nod->num_link; i++)
    {
        sprintf(data, "%s\nFORWARD\t%d\t%ld", data, i, nod->forward_link[i]->name);
    }
    status = create_file(file_path, data);
    if(status == FAILURE)
    {
        sprintf(logdata, "Failed to create file '%s'.", file_path);
        logger(logdata, ERROR);
        free(file_path);
        free(logdata);
//        free(node_path);
        free(data);
//        free(io);
        return FAILURE;
    }
    free(logdata);
    free(file_path);
    free(data);
    return SUCCESS;
}

//Add the table and configuration data into the file.
int add_memory_to_file(char *database, void *t, int type)
{
    TABLE *tab;
    NODE *nod;
    int status;
    char *logdata = (char *)malloc(LOGSIZE);

    if(type == TABLE_TYPE)
    {
        tab = (TABLE *)t;
        status = __memory_table_to_file(database, tab);
        if(status ==FAILURE)
            return FAILURE;
        status = __create_config_file(database, tab, type);
        if(status == FAILURE)
            return FAILURE;
    }
    else if(type==NODE_TYPE)
    {
        nod = (NODE *)t;

        char *io = (char *)malloc(sizeof(long int) + 10);
        sprintf(io, "%ld", nod->name);
//        printf("Node name is %s\n", io);
        status = create_type(database, NODE_TYPE, io);
        if(status == FAILURE)
        {
            sprintf(logdata, "Failed to write the data in the file. Node name is %ld.", nod->name);
            logger(logdata, ERROR);
            free(logdata);
            return FAILURE;
        }

        char *node_path = get_node_path(database, io);
        char *file_path = (char *)malloc(strlen(node_path) + strlen(io) + PATH_EXTRA_SIZE);
        sprintf(file_path, "%s/Data/content", node_path);
        status = create_file(file_path, nod->data);

        if(status == FAILURE)
        {
            sprintf(logdata, "Failed to create file '%s'.", file_path);
            logger(logdata, ERROR);
            free(file_path);
            free(logdata);
            free(node_path);
            free(io);
            return FAILURE;
        }

        status = __node_config_to_file(nod, node_path);
        if(status == FAILURE)
        {
            sprintf(logdata, "Failed to add node from memory to file. %s", file_path);
            logger(logdata, ERROR);
            free(file_path);
            free(logdata);
            free(io);
            free(node_path);
            return FAILURE;
        }

        free(file_path);
        free(io);
        free(node_path);
    }
    free(logdata);
    return SUCCESS;
}


TABLE *create_table(char *name, int number_of_columns, struct column_type *col_type[])
{
    int i;
    TABLE *t = initialize_table(name, number_of_columns);			//Initialize table first.
	//Configure the columns mode and datatype.
	for (i = 0; i < t->total_column; i++)
    {
        t->c_type[i]->column_mode = col_type[i]->column_mode;
		t->c_type[i]->column_datatype = col_type[i]->column_datatype;
        free(col_type[i]);
	}

	return t;
}

int table_save(char *database, TABLE *t)
{
    int status = add_memory_to_file(database, (void *)t, TABLE_TYPE);
    return status;
}

//Remove the table/node entirely from the memory and database folder.
int remove_database_table(char *database, TABLE *tab)
{
    //Remove the table/node entirely from the memory and database folder.

    delete_all_column_files(database, tab->name);

    if(delete_table_files(database, tab->name) == FAILURE)
        return FAILURE;
    if(delete_table(database, tab->name) == FAILURE)
        return FAILURE;

    //Deintialize the table in the memory
    deinitialize_table(tab);
    return SUCCESS;
}

//Reinitialize the table by parsing the data in file and write to the allocated memory.
TABLE *reinitialize_table(char *database, TABLE *t)
{
    int row_count, column_count, total_columns = 0;
    ROW *row;
    char name[15];
    strcpy(name, t->name);

    deinitialize_table(t);

    char ***column_data = read_table_columns(database, name);

    //Get total number of columns by reading through each files.
    while(1)
    {
        if(column_data[total_columns][0] == NULL) //Verify if the pointer reached the last column.
            break;
        total_columns++;
    }

    //Parse the data fetched from the file and write to the memory.
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
        //Creating an array referencing the rows containing data from each row.
        while(column_count < total_columns)
        {
            if(column_data[column_count][row_count] == NULL) //Verify if the pointer reached the last column.
                break;
            data[column_count] = (char *)malloc(strlen(column_data[column_count][row_count]) + 1);
            sprintf(data[column_count], "%s", column_data[column_count][row_count]);
            column_count++;
        }
        row = create_row(t->total_column, &*data);
        add_row_in_table(t, row);
        row_count++;
    }

    //Free all the memory used for reading the files.
    column_count = 0;
    while(column_count <= total_columns)
    {
        row_count = 0;
        while(1)
        {
            if(column_data[column_count][row_count] == NULL)
                break;
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

        //Free data read from files after writing into the files.
        if(strcmp(data_config[row_count], name)==0)
        {
            t->c_type[column_count]->column_datatype = atoi(data_config[row_count + 2]);
            t->c_type[column_count]->column_mode = atoi(data_config[row_count + 1]);
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

//Remove one column by passing column name in the parameter.
int __remove_table_column(char *database, TABLE *tab, char *c_name)
{
    int i, status = SUCCESS;
    int total_column;
    char *curr_col_name, *next_col_name;

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
                tab->c_type[i]->column_datatype = tab->c_type[i + 1]->column_datatype;
                tab->c_type[i]->column_mode = tab->c_type[i + 1]->column_mode;
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

TABLE *remove_column(char *database_name, TABLE *t, char* column_name)
{
    int status = __remove_table_column(database_name, t, column_name);
    if(status == FAILURE)
    {
        add_memory_to_file(database_name, t, TABLE_TYPE);
        return NULL;
    }
    t = reinitialize_table(database_name, t);
    return t;
}


//Remove one or more rows from table and database files.
int remove_table_rows(char *database, TABLE *t, int row_position[], int no_of_rows, int change_scope)
{
    sprintf(logdata, "remove_table_rows: Number of rows to be deleted: %d.", no_of_rows);
    logger(logdata, DEBUG);


    int position;
    if(no_of_rows > 1)
    {
        sprintf(logdata, "remove_table_rows: Deleting multiple rows");
        logger(logdata, DEBUG);
        delete_multiple_rows_in_table(t, row_position, no_of_rows);
        sprintf(logdata, "remove_table_rows: Deleted multiple rows");
        logger(logdata, DEBUG);
    }
    else
    {
        position = row_position[0];
        sprintf(logdata, "remove_table_rows: Deleting one row.");
        logger(logdata, DEBUG);
        delete_row_in_table(t, position);
        sprintf(logdata, "remove_table_rows: Deleted one row");
        logger(logdata, DEBUG);
    }

    if(change_scope == DEFAULT)
    {
        sprintf(logdata, "remove_table_rows: Deleted one row");
        logger(logdata, DEBUG);
        add_memory_to_file(database, (void *)t, TABLE_TYPE);
    }

    return SUCCESS;
}

/*===================Node Libraries===========================================*/

int node_save(char *database_name, NODE *n)
{
    int status = add_memory_to_file(database_name, (void *)n, NODE_TYPE);
    return status;
}

int node_tree_save(char *database_name, NODE *n)
{
    int i, num_nodes, status;
    char *logdata = (char *)malloc(LOGSIZE);
    int pid, child_status = 0;
    int parent_status = 1;
    int exit_status = 0;

    status = delete_node_folder(database_name);
    if(status == FAILURE)
    {
        return FAILURE;
    }

    while(1)
    {
        add_memory_to_file(database_name, (void *)n, NODE_TYPE);

        i = 0;
        num_nodes = n->num_link;
        if(num_nodes == 0)
        {
            exit_status = 1;
        }

        while(i < num_nodes)
        {
            pid = fork();
            if(pid == 0)
            {
                sprintf(logdata, "%d: Created the child process: %d.", getppid(), getpid());
                logger(logdata, INFO);
                child_status = 1;
                parent_status = 0;
                n = n->forward_link[i];
//                printf("FORK=> Child: %d ----- Parent: %d.\n", child_status, parent_status);
                break;
            }
            else
            {
                parent_status = 1;
//                printf("NOT FORK=> Child: %d ----- Parent: %d.\n", child_status, parent_status);
                i++;
            }
        }

        if(parent_status == 1 && child_status == 0)
        {
            break;
        }

        if((child_status == 1 && parent_status == 1) || (child_status == 1 && exit_status == 1))
        {
            sprintf(logdata, "%d: Exiting the child process: %d.", getppid(), getpid());
            logger(logdata, INFO);
            while((pid = wait(&status)) > 0);
            exit(SUCCESS);
        }

    }

    sprintf(logdata, "Waiting for all child process: %d.", getpid());
    logger(logdata, INFO);
    while((pid = wait(&status)) > 0);
    sprintf(logdata, "Waiting for all child process finished: %d.", getpid());
    logger(logdata, INFO);

    free(logdata);
    return SUCCESS;
}

/* If the mflag is true then that means the database files will not be deleted else the database file will also get deleted. */
int node_delete(char *database, NODE *n, bool mflag)
{
    int status;
    char *logdata = (char *)malloc(LOGSIZE);

    if(n->root_link == n && n->num_link != 0)
    {
        sprintf(logdata, "Cannot delete root node when branch nodes are still present.");
        logger(logdata,INFO);
        free(logdata);
        return FAILURE;
    }
    else if(n->root_link == n && n->num_link == 0)
    {
        sprintf(logdata, "Deleting root node..");
        logger(logdata,INFO);
        if(mflag == FALSE)
        {
            status = node_delete_file(database, n->name);
            if(status == FAILURE)
            {
                sprintf(logdata, "Unable to delete the root node. Hence reverting back.");
                logger(logdata,ERROR);
                node_save(database, n);
                free(logdata);
                return status;
            }
        }

        node_delete_memory(n);
        free(logdata);
        return SUCCESS;
    }

    if(mflag == FALSE)
    {
        status = node_delete_file(database, n->name);
        if(status == FAILURE)
        {
            sprintf(logdata, "Unable to delete the node. Hence reverting back.");
            logger(logdata,ERROR);
            node_save(database, n);
            free(logdata);
            return status;
        }
    }

    int total_links = n->num_link;
    int i = 0;

    NODE *previous_node = n->previous_link;
    NODE **temp_link = (NODE **)malloc(sizeof(NODE *) * total_links);
    while(i < total_links)
    {
        temp_link[i] = n->forward_link[i];
        i++;
    }
    i = 0;
    while(i < total_links)
    {
        node_link(previous_node, temp_link[i]);
        i++;
    }
    free(temp_link);


    total_links = previous_node->num_link;
    if(total_links - 1 != 0)
    {
        temp_link = (NODE **)malloc(sizeof(NODE *) * (total_links - 1));
    }
    i = 0;
    while(previous_node->forward_link[i] != n)
    {
        temp_link[i] = previous_node->forward_link[i];
        i++;
    }

    while(i < total_links - 1)
    {
        temp_link[i] = previous_node->forward_link[i + 1];
        i++;
    }

    free(previous_node->forward_link);
    previous_node->num_link--;
    if(previous_node->num_link == 0)
        previous_node->forward_link = NULL;
    else
        previous_node->forward_link = temp_link;


    node_delete_memory(n);

    free(logdata);
    return SUCCESS;
}


typedef struct node_agruments
{
    char *database;
    NODE *n;
} NODE_ARG;

int node_delete_arg(NODE_ARG *arg, bool mflag)
{
    char *database = arg->database;
    NODE *n = arg->n;
    int status = node_delete(database, n, mflag);
    return status;
}

/*mflag -> If action is required only */
int node_delete_tree(char *database, NODE *n, bool mflag)
{
    pthread_t thread_id;
    int status, i, total_link;
    char *logdata = (char *)malloc(LOGSIZE);
    NODE_ARG *arg = (NODE_ARG *)malloc(sizeof(NODE_ARG));
    NODE *previous_node = n->previous_link;


    while(1)
    {
        if(n->root_link == n && n->num_link == 0)
        {
//            if(data_mode == SYNTAX_DATA_MODE)
//                free_syntax_parameter((C_SYNTAX *)n->data);
            node_delete(database, n, mflag);
            free(logdata);
            free(arg);
            return status;
        }
        else
        {
            total_link = n->num_link;
            if(total_link == 0)
                break;

            NODE **child_nodes = (NODE **)malloc(sizeof(NODE **) * total_link);
            i = 0;
            while(i < total_link)
            {
                child_nodes[i] = n->forward_link[i];
                i++;
            }

            i = 0;
            while(i < total_link)
            {
//                if(data_mode == SYNTAX_DATA_MODE)
//                    free_syntax_parameter((C_SYNTAX *)child_nodes[i]->data);
                status = node_delete(database, child_nodes[i], mflag);
                i++;
            }
            free(child_nodes);
        }
    }
//    if(data_mode == SYNTAX_DATA_MODE)
//        free_syntax_parameter((C_SYNTAX *)n->data);
    node_delete(database, n, mflag);
    if(previous_node != NULL)
        node_save(database, previous_node);

    free(logdata);
    free(arg);
    return SUCCESS;
}

NODE *__node_reinitialize(char *database, long int name, int type)
{
    char *node_name = (char *)malloc(PATH_EXTRA_SIZE);
    char **node_data, *ptr;
    int i;
    NODE *n;

    //Create root node by parsing the rot file.
    n = initialize_node(type);

    //Get the values of the data in the node.
    sprintf(node_name, "%ld", name);
    node_data = (char **)malloc(sizeof(char *));
    node_data[0] = node_content_read_file(database, node_name);
    if(node_data[0] == NULL)
        return FAILURE;
    n->data = (char *)malloc(strlen(node_data[0])+1);
    strcpy(n->data, node_data[0]);
    n->name = strtol(node_name, &ptr, 10);
    free(node_data[0]);
    free(node_data);

    i = 0;
    node_data = node_config_read_file(database, node_name);

    //Get the config values in the config file.
    i = 0;
    while(node_data[i] != '\0')
    {
        if(strcmp(node_data[i], "TYPE")==0)
        {
            n->type = atoi(node_data[i + 1]);
            free(node_data[i]);
            i = i + 1;
        }
        free(node_data[i]);
        i++;
    }
    free(node_data);
    free(node_name);
    return n;
}

NODE *__node_tree_reinitialize(char *database, NODE *pn)
{
    //Verify number of forward link available.
    int i = 0;
    int total_count = 0;
    char *ptr;
    long int name;
    char *node_name = (char *)malloc(PATH_EXTRA_SIZE);
    sprintf(node_name, "%ld", pn->name);
    char **node_data = node_config_read_file(database, node_name);
    free(node_name);
    NODE *cn;

    while(node_data[i] != '\0')
    {
        if(strcmp(node_data[i], "FORWARD")==0)
        {
            total_count++;
        }
        i++;
    }

    if(total_count == 0)
    {
        i = 0;
        while(node_data[i] != '\0')
        {
            free(node_data[i]);
            i++;
        }
        free(node_data[i]);
        free(node_data);
        return pn;
    }

//    n->num_link = total_count;
//    n->forward_link = (NODE **)malloc(sizeof(NODE *) * total_count);
    i = 0;
    while(node_data[i] != '\0')
    {
        if(strcmp(node_data[i], "FORWARD")==0)
        {
            name = strtol(node_data[i+2], &ptr,10);
            free(node_data[i]);
            free(node_data[i+1]);
            cn = __node_reinitialize(database, name, BRANCH_NODE);      //Reinitialize the Node.
            node_link(pn, cn);
            cn = __node_tree_reinitialize(database, cn);        //Pass the new node recursively into the same function to attach child node.
            i = i + 2;
        }
        free(node_data[i]);
        i++;
    }
//    free(node_data[i]);
    free(node_data);
    return cn->root_link;
}

NODE *node_tree_reinitialize(char *database)
{
    NODE *n;
    n = __node_reinitialize(database, 1, ROOT_NODE);
    n = __node_tree_reinitialize(database, n);
    return n->root_link;
}
/*===================Node Libraries(Completed)===========================================*/













