#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char *inputString(FILE* fp, size_t size) {
//The size is extended by the input with the value of the provisional
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str)*size);//size is start size
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(*str)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(*str)*len);
}

void printCurrentDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf(ANSI_COLOR_MAGENTA "%s/\n", cwd);
    printf(">>> " ANSI_COLOR_RESET);
}

void builtInCommands(char input[]){
    char *command[20];
    int i = 0;
    char *tmp;

    command[i] = strtok(input, " ");
    while(command[i])
        command[++i] =  strtok(NULL, " ");
    if(!strcmp(command[0], "cd")){ 
        chdir(command[1]); 
    }
    else{
        pid_t pid = fork();
        if (pid == 0){ //child process executing built-in commands 
            if( execvp(command[0], command) < 0)
                fprintf(stderr, "command not found.\n");    
            exit(0);
        }
        else{
            wait(NULL);
            return;
        }
    }
}

int main() {
    int running = 1;
    char *line;
    while(running) {
        printCurrentDir();
        line = inputString(stdin, 10);
        builtInCommands(line);
        free(line);
    }
    return(0);
}