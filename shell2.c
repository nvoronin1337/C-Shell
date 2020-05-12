#include <stdio.h>  // for I/O
#include <stdlib.h> // for free()
#include <string.h> // for strcmp(), strtok()

#include <errno.h> // for perror()

// for fork(), execvp(), wait()
#include <sys/types.h>  
#include <unistd.h>
#include <sys/wait.h>

#define EXIT_USER 5
#define QUIT "quit"

int parse_input_line(char *, char **);
void run_commands(int, char **);
void execute(char **);
void remove_spaces(char *);
int read_file(FILE*, char **);

int read_file(FILE* file, char **lines){
    char* file_input = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;

    int i = 0;
    
    while(lineSize != -1){
        lineSize = getline(&lines[i], &len, file);
        i++;
    }

    return i;
}

int parse_input_line(char* input, char** commands){
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

    if(command){
        free(command);
        command = NULL;
    }
    return i;
}

void run_commands(int size, char** commands){
    const char* ARG_SEPARATOR = " ";

    char** args = malloc(512 * sizeof(char *));
    char* arg;

    for(int k = 0; k < size; k++){    
        arg = strtok(commands[k], ARG_SEPARATOR);

        int l = 0;
        while(arg){
            remove_spaces(arg);

            if(!strcmp(arg, QUIT)){
                exit(EXIT_USER);
            }
            args[l] = arg;
            l++;
            arg = strtok(NULL, ARG_SEPARATOR);
        }
        
        args[l] = NULL;
        execute(args);   
        printf("/n"); 
    }
    free(args);
    args = NULL;
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

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

int main(int argc, char* argv[]){
    switch(argc){
    case 1:
    {
        char* input = NULL;
        while(1){
            size_t len = 0;
            ssize_t lineSize = 0;

            printf("shell -> ");
            lineSize = getline(&input , &len, stdin);

            if(lineSize-1 != 0){
                char** commands = malloc(512 * sizeof(char *));
                int size = parse_input_line(input, commands);
                run_commands(size, commands);
                free(commands);
                commands = NULL;
            }
        }
        break;
    }
    case 2:
    {
        char** lines = malloc(128 * sizeof(char *));
        char** commands = malloc(512 * sizeof(char *));

        FILE* file;
        if(file = fopen(argv[1], "r")){
            int total_lines = read_file(file, lines);
           
            for(int i = 0; i < total_lines; i++){           
                int commands_in_line = parse_input_line(lines[i], commands);
                run_commands(commands_in_line, commands);
            }

            fclose(file);
        }else{
            perror("File not found!\n");
        }

        free(lines);
        lines = NULL;

        free(commands);
        commands = NULL;

        break;
    }
    default:
        perror("Invalid number of arguments!\n");
        return(EXIT_FAILURE);
    }
    return(EXIT_SUCCESS);
}
