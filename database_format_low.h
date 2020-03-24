#define MAX_COLUMNS 1000
#define MAX_ROWS 1000

//struct rows
//{
//    char *cell_data[MAX_COLUMNS];
//    int rows_position;
//};
//typedef struct rows ROW;
//
//struct table
//{
//    ROW rows[MAX_ROWS];
//};


struct rows
{
    char *cell_data[MAX_COLUMNS];
};
typedef struct rows ROW;

ROW *initialize_row()
{
    ROW *row = (ROW *)malloc(sizeof(ROW));
    return row;
}

struct table
{
    ROW *rows[MAX_ROWS];
    int total_rows;
    int total_column;
};

typedef struct table TABLE;

TABLE *initialize_table()
{
    TABLE *t = (TABLE *)malloc(sizeof(TABLE));
    t->total_rows = 0;
    t->total_column = 0;
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
            if(t->rows[i]->cell_data[j] != NULL)
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

ROW *create_row(int num, ...)
{
    va_list valist;
    int i;
    ROW *row = initialize_row();

    va_start(valist, num);

    for(i=0; i<num; i++)
    {
        row->cell_data[i] = va_arg(valist, char *);
    }

    return row;
}

void add_row_to_table(TABLE *t, ROW *r)
{
    int i = t->total_rows;
    t->rows[i] = r;
    t->total_rows = i + 1;
}

