#ifndef PROGRAMMING_LOW_H_INCLUDED
#define PROGRAMMING_LOW_H_INCLUDED

#define PARAM_SIZE 30
#define DELIMITOR_SYNTAX "$"

#endif // PROGRAMMING_LOW_H_INCLUDED

struct syntax{
        char command[20];
        char **parameter;
        char **typecast;
        short int type;
    };

typedef struct syntax C_SYNTAX;

char *syntax_to_data(C_SYNTAX *sy)
{
    char *data;
    int cnt, i = 0;
    if(sy->parameter[i] != '\0')
    {
        i++;
    }

    cnt = (i * 2) + 2;
    data = (char *)malloc((sizeof(char) + 30) * cnt);
    sprintf(data, "%s%s", sy->command, DELIMITOR_SYNTAX);
    sprintf(data, "%s%d%s", data, sy->type, DELIMITOR_SYNTAX);
    i = 0;
    while(1)
    {
        if(sy->parameter[i] != '\0')
        {
            if(sy->typecast != '\0')
            {
                if(sy->typecast[i] != '\0')
                    sprintf(data, "%s%s%s", data, sy->typecast[i], DELIMITOR_SYNTAX);
                else
                    sprintf(data, "%sNULL%s", data, DELIMITOR_SYNTAX);
            }
            else
                sprintf(data, "%sNULL%s", data, DELIMITOR_SYNTAX);

            sprintf(data, "%s%s%s", data, sy->parameter[i], DELIMITOR_SYNTAX);
            i = i + 1;
        }
        else
            break;
    }
    sprintf(data, "%sEND", data);

    return data;
}

C_SYNTAX *data_to_syntax(char *data_unformat)
{
    char *temp_unformat;
    C_SYNTAX *sy = (C_SYNTAX *)malloc(sizeof(C_SYNTAX));
    sscanf(data_unformat, "%[^$]s", sy->command);
    data_unformat = data_unformat + strlen(sy->command) + sizeof(DELIMITOR_SYNTAX) - 1;

    char *temp_data = (char *)malloc(50);
    sscanf(data_unformat, "%[^$]s", temp_data);
    sy->type = atoi(temp_data);
    data_unformat = data_unformat + strlen(temp_data) + sizeof(DELIMITOR_SYNTAX) - 1;

    int  i = 0;
    bool type_cast_flag = false;
    temp_unformat = data_unformat;
    while(strcmp(data_unformat, "END") != 0)
    {
        //Type cast flag
        sscanf(data_unformat, "%[^$]s", temp_data);
        if(strcmp(temp_data, "NULL") != 0)
            type_cast_flag = true;
        data_unformat = data_unformat + strlen(temp_data) + sizeof(DELIMITOR_SYNTAX) - 1;

        //Parameter value
        sscanf(data_unformat, "%[^$]s", temp_data);
        data_unformat = data_unformat + strlen(temp_data) + sizeof(DELIMITOR_SYNTAX) - 1;
        i = i + 1;
    }

    data_unformat = temp_unformat;
    sy->parameter = (char **)malloc(sizeof(char *) * (i + 1));
    if(type_cast_flag == true)
        sy->typecast = (char **)malloc(sizeof(char *) * (i + 1));
    else
        sy->typecast = '\0';
    i = 0;
    while(strcmp(data_unformat, "END") != 0)
    {
        //Type cast value
        sscanf(data_unformat, "%[^$]s", temp_data);
        if(strcmp(temp_data, "NULL") == 0 && sy->typecast != '\0')
            sy->typecast[i] = '\0';
        else if(strcmp(temp_data, "NULL") != 0 && sy->typecast != '\0')
            strcpy(sy->typecast[i], temp_data);
        data_unformat = data_unformat + strlen(temp_data) + sizeof(DELIMITOR_SYNTAX) - 1;

        //Parameter value
        sscanf(data_unformat, "%[^$]s", temp_data);
        sy->parameter[i] = (char *)malloc(strlen(temp_data) + 1);
        strcpy(sy->parameter[i], temp_data);
        data_unformat = data_unformat + strlen(temp_data) + sizeof(DELIMITOR_SYNTAX) - 1;
        i = i + 1;
    }

    if(sy->typecast != '\0')
    {
        sy->typecast[i] = '\0';
    }
    sy->parameter[i] = '\0';
    free(temp_data);
    return sy;
}

char **__parameter(C_SYNTAX *sy)
{
    char **param;
    int cnt = 0;

    while(sy->parameter[cnt] != '\0')
        cnt++;
    param = (char **)malloc(sizeof(char *) * (cnt + 2));


    int i = 1;
    param[0] = (char *)malloc(15);
    sprintf(param[0], "%s", sy->command);
    while(sy->parameter[i - 1] != '\0')
    {
        param[i] = (char *)malloc(PARAM_SIZE);
        if(sy->typecast != '\0')
        {
            if(sy->typecast[i - 1] != '\0')
                sprintf(param[i], "(%s)%s", sy->typecast[i - 1], sy->parameter[i - 1]);
            else
                sprintf(param[i], "%s", sy->parameter[i - 1]);
        }
        else
            sprintf(param[i], "%s", sy->parameter[i - 1]);
        i++;
    }
    param[i] = '\0';

    return param;
}

void free_syntax_parameter(C_SYNTAX *sy)
{
    int i = 0;
    int cnt = 0;

    while(sy->parameter[cnt] != '\0')
        cnt++;

    while(i < cnt)
    {
        free(sy->parameter[i]);
        i++;
    }
    free(sy->parameter);
}
void free_syntax_creation(C_SYNTAX *sy)
{
    free_syntax_parameter(sy);
    free(sy);
}

char *syntax_header()
{
    char *header = (char *)malloc(10000);

    strcpy(header, "#ifndef HEADERS_H_INCLUDED\n"
                    "#define HEADERS_H_INCLUDED\n"
                    "//Builtin Header files\n"
                    "#include <stdio.h>\n"
                    "#include <stdlib.h>\n"
                    "#include <stdbool.h>\n"
                    "#include <string.h>\n"
                    "#include <stdarg.h>\n"
                    "#include <time.h>\n"
                    "#include <sys/socket.h>\n"
                    "#include <sys/types.h>\n"
                    "#include <sys/wait.h>\n"
                    "#include <netinet/in.h>\n"
                    "#include <arpa/inet.h>\n"
                    "#include <unistd.h>\n"
                    "#include <pthread.h>\n"
                    "#include <errno.h>\n"
                    "#include <sys/ipc.h>\n"
                    "#include <sys/shm.h>\n"
                    "#include <sys/mman.h>\n"
                    "#include <sys/stat.h>\n"
                    "#include <sys/fcntl.h>\n"
                    "#endif // HEADERS_H_INCLUDED\n\n");

    return header;
}

char *syntax_main()
{
    char *m = (char *)malloc(50);

    strcpy(m, "void main()\n{\n");
    return m;

}

char *syntax_creation(C_SYNTAX *sy)
{
    char *command = (char *)malloc(10000);
    char *logdata = (char *)malloc(LOGSIZE);
    int flag = SUCCESS;
//    sprintf(command, "%s(", sy->command);
//    printf("1: %s\n", sy->command);

    char **param = __parameter(sy);

    int i = 1;
    if(sy->type == CODE_FUNCTION)
    {
        sprintf(command, "%s(", param[0]);
        while(param[i] != '\0')
        {
            if(i == 1)
                sprintf(command, "%s%s", command, param[i]);
            else
                sprintf(command, "%s, %s", command, param[i]);
            i++;
        }
        sprintf(command, "%s);\n", command);
    }
    else if(sy->type == CODE_DEFINITION)
    {
        sprintf(command, "%s", param[0]);
        while(param[i] != '\0')
        {
            if(i == 1)
                sprintf(command, "%s %s", command, param[i]);
            else
                sprintf(command, "%s, %s", command, param[i]);
            i++;
        }
        sprintf(command, "%s;\n", command);
    }
    else if(sy->type == CODE_INITIALIZE)
    {
        sprintf(command, "%s = %s;\n", param[0], param[1]);
    }
    else
    {
        sprintf(logdata, "Syntax type error found.");
        logger(logdata, ERROR);
        flag = FAILURE;
    }

    i = 0;
    while(param[i] != '\0')
    {
        free(param[i]);
        i++;
    }

    free(logdata);
    free(param);
    if(flag == FAILURE)
        return FAILURE;
    return command;
}

char *syntax_end()
{
    char *e = (char *)malloc(2);
    strcpy(e, "}");
    return e;
}

typedef struct syntax R_PROG;
int run(R_PROG *prog)
{
    char **param = __parameter(prog);
    char *command = (char *)malloc(1000);

    sprintf(command, "%s", param[0]);
    int i = 1;
    while(param[i] != '\0')
    {
        sprintf(command, "%s %s", command, param[i]);
        i++;
    }

    i = 0;
    while(param[i] != '\0')
    {
        free(param[i]);
        i++;
    }
    free(param);

    int status = system(command);

    return status;
}

void syntax_print(C_SYNTAX *sy)
{
    printf("Command: %s\n", sy->command);
    int i = 0;
    while(sy->parameter[i]!= NULL)
    {
        printf("Parameter: %s\n", sy->parameter[i]);
        i++;
    }
    printf("Type: %d\n", sy->type);
    int total_count = i;
    i = 0;
    if(sy->typecast != NULL)
    {
        while(i < total_count)
        {
            printf("Typecase: %s\n", sy->typecast[i]);
            i++;
        }
    }
}
