#define MAX_COLUMNS 1000
#define MAX_ROWS 1000

struct column_type
{
    char column_datatype[15];
    char column_mode[15];
};

struct rows
{
    char *cell_data[MAX_COLUMNS];
    int total_columns;
};
typedef struct rows ROW;

ROW *initialize_row()
{
    ROW *row = (ROW *)malloc(sizeof(ROW));
    sprintf(logdata, "Initialized row successfully.");
    logger(logdata, DEBUG);
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

struct node_t
{
    int a;
};

typedef struct node_t NODE;


TABLE *initialize_table(char name[], int total_column)
{
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
    return t;
}

void table_print(TABLE *t)
{
    int i, j = 0;

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
    return row;
}

void add_row_in_table(TABLE *t, ROW *r)
{
    int i = t->total_rows;
    t->rows[i] = r;
    t->total_rows = i + 1;
    sprintf(logdata, "Added row to the table %s successfully.", t->name);
    logger(logdata, DEBUG);
}

void delete_row_in_table(TABLE *t, int row_position)
{
    int i, column_count;

    sprintf(logdata, "Deleting the row with index: %d.", row_position);
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
    sprintf(logdata, "Deleted the row with index: %d.", row_position);
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

    sprintf(logdata, "Number entries to be deleted is %ld.", size_q);
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
