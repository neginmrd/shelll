#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_LINES_IN_FILE 500
#define MAX_CHARACTERS_IN_LINE 100

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

void storeCommand(char input[]){ 
    FILE *fp;

    fp = fopen("history.txt", "a");
 
    if(fp == NULL) {
        fprintf(stderr, "Could not open file.\n"); 
        return;
    }
    fflush(stdin);
    fputs(input,fp);
    fputs("\n",fp);
    fclose(fp);
}


/////////////// custom commands
void headlines(char lines[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE], int count) {
    char *token;
    char line[MAX_CHARACTERS_IN_LINE];
    for (int i = 0; i < count; i++) {
        strcpy(line, lines[i]);
        token = strtok(line, " ");
        printf("%s\n", token);
    }
    return;
}

void head(char lines[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE], int num, int count) {
    for (int i = 0; i < count && i < num; i++)
        printf("%s\n", lines[i]);
    return;
}

void howmanylines(char lines[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE], int count) {
    printf("%d\n", count);
    return;
}

int isOwnCommand(char input[]){
    char ownCommands[6][15] = {"headlines", "freq", "noSpace", "noComment", "howmanylines", "head10"};
    int i = 0;
    while(i < 6){
        if(!strcmp(ownCommands[i], input)){
            printf("found it in own commands.\n");
            return(1);
        }
        i++;
    }
    return(0);
}

int processOwnCommands(char input[]) {
    char *command = strtok(input, " ");
    char *filename = strtok(NULL, " ");
    if (!isOwnCommand(command)) return(0);

    // read lines from file and put them in array
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL){
        printf("Could not open file.\n");
        return(1);
    }
    char buffer[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE];
    int count = 0; // number of lines
    char tmp[MAX_CHARACTERS_IN_LINE];
    while(fgets(tmp, MAX_CHARACTERS_IN_LINE, fptr) != NULL){
        tmp[strcspn(tmp, "\n")] = 0;
        strcpy(buffer[count], tmp);
        count++;
    }
    fclose(fptr);

    if (strcmp(command, "headlines") == 0) headlines(buffer, count);
    else if (strcmp(command, "head10") == 0) head(buffer, 10, count);
    else if (strcmp(command, "howmanylines") == 0) howmanylines(buffer, count);

    else printf("other commands.\n");

    return(1);
}

int main() {
    int running = 1;
    char *line;
    while(running) {
        printCurrentDir();
        line = inputString(stdin, 10);
        storeCommand(line);
        if (!processOwnCommands(line))
            builtInCommands(line);
        free(line);
    }
    return(0);
}