/*====================> Table Code <========================*/

struct column_type
{
    short int column_datatype;
    short int column_mode;
};

struct rows
{
    char *cell_data[MAX_COLUMNS];
    int total_columns;
};
typedef struct rows ROW;

ROW *initialize_row()
{
    char *logdata = (char *)malloc(LOGSIZE);
    ROW *row = (ROW *)malloc(sizeof(ROW));
    sprintf(logdata, "Initialized row successfully.");
    logger(logdata, DEBUG);
    free(logdata);
    return row;
}

struct table
{
    ROW *rows[MAX_ROWS];
    char name[15];
    char table_type[15];
    int total_rows;
    int total_column;
    struct column_type *c_type[];
};

typedef struct table TABLE;


TABLE *initialize_table(char name[], int total_column)
{
    char *logdata = (char *)malloc(LOGSIZE);
    if(total_column > MAX_COLUMNS)
    {
        sprintf(logdata, "initialize_table: Maximum number columns exceeds more than %d allowed.", MAX_COLUMNS);
        logger(logdata, ERROR);
        free(logdata);
        return FAILURE;
    }
    TABLE *t = (TABLE *)malloc(sizeof(TABLE) + sizeof(ROW) + sizeof(struct column_type));
    int i;
    t->total_rows = 0;
    t->total_column = total_column;
    strcpy(t->name, name);
    for(i = 0; i < total_column; i++)
    {
        t->c_type[i] = (struct column_type *)malloc(sizeof(struct column_type));
    }
    sprintf(logdata, "Initialized table %s successfully.", t->name);
    logger(logdata, DEBUG);
    free(logdata);
    return t;
}

void table_print(TABLE *t)
{
    int i, j = 0;

    printf("Table name:  %s\n", t->name);
    printf("Number columns: %d\nNumber rows:%d\n", t->total_column, t->total_rows);
    printf("Table type: %s\n", t->table_type);
    printf("Mode\tDatatype\n");
    for(i = 0; i < t->total_column; i++)
    {
        printf("%d\t%d\n", t->c_type[i]->column_mode, t->c_type[i]->column_datatype);
    }

    for(i = 0; i < t->total_rows; i++)
    {
        j=0;
        while(1)
        {
            if(j < t->total_column)
            {
                printf("%d-%d : %s\t", i, j ,t->rows[i]->cell_data[j]);
                j++;
            }
            else
                break;
        }

        printf("\n");
    }
}

ROW *create_row(int num, char **data)
{
    char *logdata = (char *)malloc(LOGSIZE);
    int i;
    ROW *row = initialize_row();

    for(i=0; i<num; i++)
    {
        row->cell_data[i] = (char *)malloc(strlen(data[i]) + 1);
        strcpy(row->cell_data[i], data[i]);
        free(data[i]);
    }
    sprintf(logdata, "Created row successfully.");
    logger(logdata, DEBUG);
    free(logdata);
    return row;
}

int add_row_in_table(TABLE *t, ROW *r)
{
    char *logdata = (char *)malloc(LOGSIZE);
    int i = t->total_rows;
    if(i > MAX_ROWS)
    {
        sprintf(logdata, "initialize_table: Maximum number of rows will exceeds more than %d allowed.", MAX_ROWS);
        logger(logdata, ERROR);
        free(logdata);
        return FAILURE;
    }
    t->rows[i] = r;
    t->total_rows = i + 1;
    sprintf(logdata, "add_row_in_table: Added row to the table %s successfully.", t->name);
    logger(logdata, DEBUG);
    free(logdata);
    return SUCCESS;
}

void delete_row_in_table(TABLE *t, int row_position)
{
    int i, column_count;

    sprintf(logdata, "delete_row_in_table: Deleting the row number %d.", row_position);
    logger(logdata, DEBUG);
    ROW *temp_row = t->rows[row_position];

    for(i = row_position; i < t->total_rows - 1; i++)
    {
        t->rows[i] = t->rows[i+1];
    }

    t->total_rows -= 1;

    for(column_count=0; column_count < t->total_column; column_count++)
    {
        free(temp_row->cell_data[column_count]);
    }
    free(temp_row);
    sprintf(logdata, "delete_row_in_table: Deleted the row %d.", row_position);
    logger(logdata, DEBUG);
}

void delete_multiple_rows_in_table(TABLE *t, int row_position[], size_t num)
{
    int c = 0, i;
    int c_temp = 0;

    //Checking the duplicate position details.
    for(c = 0; c < num - 1; c++)
    {
        c_temp = c + 1;
        while(c_temp < num)
        {
            if(row_position[c] == row_position[c_temp])
            {
                i = c_temp;
                while(i < num - 1)
                {
                    row_position[i] = row_position[i + 1];
                    i++;
                }
                num--;
            }
            else
                c_temp++;
        }
    }

    c = 0;
    c_temp = 0;
    size_t size_q = num;

    sprintf(logdata, "delete_multiple_rows_in_table: Actual number of entries that will be deleted after removing duplicates: %ld.", size_q);
    logger(logdata, DEBUG);

    while(1)
    {
        delete_row_in_table(t, row_position[c]);
        c++;
        if(c >= size_q)
            break;

        //Correcting the position number after deleting a row.
        for(c_temp = c; c_temp < size_q; c_temp++)
            if(row_position[c_temp] > row_position[c-1])
            {
                row_position[c_temp]--;
            }
    }
    sprintf(logdata, "delete_multiple_rows_in_table: Actual number of entries deleted: %ld.", size_q);
    logger(logdata, DEBUG);
}

void deinitialize_table(TABLE *t)
{
    int column_count, row_count, row_num = 0;
    int total_rows = t->total_rows;
    for(row_count = 0; row_count < total_rows; row_count++)
    {
        delete_row_in_table(t, row_num);
    }

    for(column_count = 0; column_count < t->total_column; column_count++)
    {
        free(t->c_type[column_count]);
    }

    free(t);
}
/*====================> Table Code <========================*/

/*====================> Node Code <========================*/
struct node_t
{
    int8_t type;
    long int name;
    char *data;
    int num_link;
    struct node_t *previous_link, **forward_link, *root_link;
};

typedef struct node_t NODE;

NODE *initialize_node(int type)
{
    NODE *n = (NODE *)malloc(sizeof(NODE));

    n->type = type;
    if(type == ROOT_NODE)
        n->name = 1;
    else
        n->name = -1;
    n->data = NULL;
    n->num_link = 0;
    n->previous_link = NULL;
    n->forward_link = NULL;
    if(type == ROOT_NODE)
        n->root_link = n;
    else
        n->root_link = NULL;
    return n;
}

void node_print(NODE *n)
{
    int i;
    printf("===========>Node Information<============\n");
    printf("Type: %d\n", n->type);
    printf("Name: %ld\n", n->name);
    if(n->data == NULL)
        printf("Data: NULL\n");
    else
        printf("Data: %s\n", n->data);
    printf("Number of Links: %d\n", n->num_link);

    if(n->root_link != n)
        printf("P_Link: %p\n", n->previous_link);
    printf("C_Link: %p\n", n);
    printf("F_Link: %p\n", n->forward_link);
    for(i=0; i < n->num_link; i++)
    {
        printf("F_Link-%d: %p\n", i, n->forward_link[i]);
    }

    printf("R_Link: %p\n", n->root_link);

    printf("===========>---------------<=============\n");
}


int node_link(NODE *p_node, NODE *c_node)
{
    int total_link = p_node->num_link + 1;

    //Assigning name to the node randomly.
    if(c_node->name < 0)
        c_node->name = (random() % time(NULL)) + time(NULL);

    int i = 0;
    NODE **temp = (NODE **)malloc(sizeof(NODE *) * total_link);
    while(i < total_link - 1)
    {
        temp[i] = p_node->forward_link[i];
        i++;
    }
    temp[i] = c_node;

    c_node->previous_link = p_node;
    c_node->root_link = p_node->root_link;
    free(p_node->forward_link);
    p_node->forward_link = temp;
    p_node->num_link = total_link;
    return SUCCESS;
}

//NODE *node_copy(NODE *n, bool exclude_data)
//{
//    NODE *new_node;
//    new_node = initialize_node(n->type);
//    memcpy((void *)new_node, (void *)n, sizeof(n));
//    new_node->name = n->name;
//    new_node->root_link = n->root_link;
//    new_node->type = n->type;
//    new_node->previous_link = n->previous_link;
//    new_node->forward_link = n->forward_link;
//    if(exclude_data == false)
//    {
//        new_node->data = (char *)malloc(strlen(n->data) + 1);
//        strcpy(new_node->data, n->data);
//    }
//    return new_node;
//}

void node_delete_memory(NODE *n)
{
    char *logdata = (char *)malloc(LOGSIZE);
//    int i = 0;
//    if(i < n->num_link)
//    {
//        free(n->forward_link[i]);
//        i++;
//    }

    n->previous_link = NULL;
    n->root_link = NULL;

    free((void *)n->forward_link);
    n->forward_link = NULL;
    sprintf(logdata, "Address: %p. Size: %ld.", n , sizeof(*n));
    logger(logdata, DEBUG);
    free((void *)n->data);
    n->data = NULL;
    free((void *)n);
    n = NULL;
    free(logdata);
}
/*====================> Node Code <========================*/























