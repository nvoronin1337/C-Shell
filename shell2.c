#include <stdio.h>  // for I/O
#include <stdlib.h> // for free()
#include <string.h> // for strcmp(), strtok()

#include <errno.h> // for perror()

// for fork(), execvp(), wait()
#include <sys/types.h>  
#include <unistd.h>
#include <sys/wait.h>

#define EXIT_USER 5

int parse_input(char *, char **);
void parse_commands(int, char **);
void execute(char **);

int parse_input(char* input, char** commands){
    size_t ln = strlen(input) - 1;

    if (*input && input[ln] == '\n') 
        input[ln] = '\0';
    
    const char* COMMAND_SEPARATOR = ";";
    
    char* command;
    command = strtok(input, COMMAND_SEPARATOR);

    int i = 0;
    while(command){
        commands[i] = command;
        command = strtok(NULL, COMMAND_SEPARATOR);
        i++;
    }

    return i;
}

void parse_commands(int size, char** commands){
    const char* ARG_SEPARATOR = " ";

    char** args = malloc(512 * sizeof(char *));
    char* arg;

    for(int k = 0; k < size; k++){    
        arg = strtok(commands[k], ARG_SEPARATOR);

        int l = 0;
        while(arg){
            args[l] = arg;
            l++;
            arg = strtok(NULL, ARG_SEPARATOR);
        }
        args[l] = NULL;
        execute(args);    
    }
    free(args);
    args = NULL;

    free(commands);
    commands = NULL;
}

void execute(char** command){
    pid_t pid;
    int return_code;

    switch((pid = fork())){
    case -1:
        perror("fork() error\n");
        exit(EXIT_FAILURE);
    case 0:
        return_code = execvp(*command, command);
        if(return_code < 0)
            return_code = EXIT_FAILURE;
        exit(return_code);
    default:
        wait(&return_code);
        if(return_code != 0){
            printf("execution of ");
            while(*command) printf("%s ", *command++);
            printf("failed!\n");
        }
    }
}

int main(int argc, char* argv[]){
    if(argc > 1){
        printf("batch mode\n");
    }else{
        while(1){
            char* input = NULL;
            size_t len = 0;
            ssize_t lineSize = 0;

            printf("shell -> ");
            lineSize = getline(&input , &len, stdin);

            if(lineSize-1 != 0){
                if(!strcmp(input, "quit\n")){
                    return(EXIT_USER);
                }

                char** commands = malloc(512 * sizeof(char *));
                int size = parse_input(input, commands);
                parse_commands(size, commands);
            }

            free(input);
            input = NULL;
        }
    }
    return(EXIT_SUCCESS);
}
