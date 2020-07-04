int code_writer(char *database, NODE *n)
{
    char *headers = syntax_header();
    char *filepath = (char *)malloc(strlen(ROOT_STORAGE) + strlen(database) + PATH_EXTRA_SIZE);
    char *database_path = get_database_path(database);
    sprintf(filepath, "%s%s", database_path, CODE_WRITER);

    //Add headers.
    printf("%s\n", filepath);
    if(create_file(filepath, headers) == FAILURE)
    {
        free(filepath);
        free(database_path);
        return FAILURE;
    }
    free(headers);

    //Add main function.
    char *m = syntax_main();
    if(append_file(filepath, m) == FAILURE)
    {
        free(filepath);
        free(database_path);
        return FAILURE;
    }
    free(m);

    //Add body content.
    char *code;
    C_SYNTAX *sy;
    while(1)
    {
        sy = data_to_syntax(n->data);
        code = syntax_creation(sy);
        if(append_file(filepath, code) == FAILURE)
        {
            free(filepath);
            free(database_path);
            return FAILURE;
        }
        free(code);
        if(n->forward_link == NULL)
            break;
        n = n->forward_link[0];
    }

    //Add the end parenthesis.
    char *e = syntax_end();
    if(append_file(filepath, e) == FAILURE)
    {
        free(filepath);
        free(database_path);
        return FAILURE;
    }
    free(e);

    free(filepath);
    free(database_path);
    return SUCCESS;
}

int code_compile(char *database, long int name)
{
    char *filepath = (char *)malloc(strlen(ROOT_STORAGE) + strlen(database) + PATH_EXTRA_SIZE);
    char *database_path = get_database_path(database);
    char *outpath = (char *)malloc(strlen(database_path) + PATH_EXTRA_SIZE);

    sprintf(filepath, "%s%s", database_path, CODE_WRITER);
    sprintf(outpath, "%s%s%ld", database_path, PROG_STORAGE, name);

    char *cmd = (char *)malloc(strlen(filepath) + 50);
    sprintf(cmd, "gcc %s -o %s", filepath, outpath);
    printf("%s\n", cmd);
    system(cmd);

    return SUCCESS;
}

NODE *create_node_syntax(C_SYNTAX *sy, int type)
{
    NODE *n = initialize_node(type);
    char *temp_data = syntax_to_data(sy);
//    n->data = stringToBinary(temp_data);
    n->data = syntax_to_data(sy);
    free_syntax_creation(sy);
    free(temp_data);
    return n;
}

