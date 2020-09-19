
NODE *compare_node(NODE *node, char *data)
{
    int cmp_status;
    printf("====> %s == %s <=====\n", node->data, data);
    cmp_status = strcmp(node->data, data);
    if(cmp_status == 0)
        return node;
    else
        return NULL;
}

NODE **sel_node;
NODE **cp_node_to_list(NODE **dp, NODE *cn)
{
    int i = 0;
    NODE **temp_dp;
    if(dp == '\0')
    {
        dp = (NODE **)malloc(sizeof(NODE *) * 2);
        dp[0] = cn;
        dp[1] = '\0';
    }
    else
    {
        while(dp[i] != '\0')
        {
            printf("Word is %d-%s.\n", i, dp[i]->data);
            i++;
        }

        temp_dp = (NODE **)malloc(sizeof(NODE *) * (i + 2));
        i = 0;
        while(dp[i] != '\0')
        {
            printf("Word-1 is %d-%s.\n", i, dp[i]->data);
            temp_dp[i] = dp[i];
            i++;
        }
        temp_dp[i] = cn;
        temp_dp[i + 1] = '\0';
        free(dp);
        dp = temp_dp;
    }

    return dp;
}

//int __nct;
void search_node(NODE *node, char *data)
{
    int i = 0;
    bool flag = FALSE;
    unsigned int total_link;
    NODE *temp_node;

//    printf("Comparing with %s and %s. \n", node->data, data);
    temp_node = compare_node(node, data);
    if(temp_node == NULL)
    {
        total_link = node->num_link;
        if(total_link != 0)
        {
            while(i < total_link)
            {
//                node_print(node->forward_link[i]);
                search_node(node->forward_link[i], data);
                i = i + 1;
            }
        }
    }
    else
    {
        if(sel_node != '\0')
            while(sel_node[i] != '\0')
            {
                if(sel_node[i] == temp_node)
                {
                    flag = TRUE;
                    break;
                }
                i++;
            }

        if(flag == FALSE)
            sel_node = cp_node_to_list(sel_node, temp_node);
    }
}

NODE *__simple_storage_char(NODE *r_node, char *data)
{
    sel_node = '\0';
    search_node(r_node, data);
    if(sel_node == '\0')
    {
        NODE *n = initialize_node(BRANCH_NODE);
        n->data = (char *)malloc(sizeof(char) * 2);
        sprintf(n->data, "%c", *data);
        node_link(r_node, n);
        return n;
    }
    return NULL;
}

NODE *simple_storage_char(NODE *r_node, char *data)
{
    NODE *n = __simple_storage_char(r_node, data);
    if(sel_node != '\0')
        free(sel_node);
    return n;
}

void node_add_word(NODE *r_node, char *data)
{
    NODE *child_n;
    int strcnt, i = 0;
    char *c;
    strcnt = strlen(data);
//    search_node(r_node, "<word>");

    NODE *n = initialize_node(BRANCH_NODE);
    n->data = (char *)malloc(7);
    strcpy(n->data, "<word>");

    while(i < strcnt)
    {
        c = (char *)malloc(sizeof(char) * 2);
        sprintf(c, "%c", data[i]);
        child_n = __simple_storage_char(r_node, c);
//        child_n = c;
        if(child_n != NULL)
        {
            node_print(child_n);
            node_link_ignore_previous(n, child_n);
        }
        else
            node_link_ignore_previous(n, sel_node[0]);
        free(c);
        free(sel_node);
        i++;
    }
    node_link(r_node, n);
}

//void node_add_word(NODE *r_node, char *data)
//{
//    free(sel_node);
//    __node_add_word(r_node, data);
//}

void simple_storage_word(NODE *r_node, char *data)
{
    sel_node = '\0';
    search_node(r_node, "<word>");
    NODE **sel_node_local = sel_node;

    if(sel_node == '\0')
    {
        node_add_word(r_node, data);
    }
    else
    {
        int i = 0, for_cnt;
        int total_links;
        char *search_data;
        NODE *n;

        while(1)
        {
            n = sel_node[i];
            if(n == '\0')
            {
                node_add_word(r_node, data);
                break;
            }
//            node_print(n);
            for_cnt = 0;
            total_links = n->num_link;
            search_data = (char *)malloc(sizeof(char) * (total_links + 1));
            while(for_cnt < total_links)
            {
                if(for_cnt == 0)
                    sprintf(search_data,"%s" , n->forward_link[for_cnt]->data);
                else
                    sprintf(search_data,"%s%s" , search_data, n->forward_link[for_cnt]->data);
                for_cnt++;
            }
            printf("=============? %s == %s \n", search_data, data);
            int status = strcmp(search_data, data);
            free(search_data);
            if(status == 0)
                break;

            i++;
        }
    }
    free(sel_node_local);
}

void simple_storage_cleanup(NODE *n)
{
    int i = 0;
    while(i < n->num_link)
    {
        node_delete_memory(n->forward_link[i]);
        i++;
    }
    node_delete_memory(n);
}
