#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>


#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_LINES_IN_FILE 500
#define MAX_CHARACTERS_IN_LINE 100

void inputString(char* str) {
    char* buf;
    buf = readline(">>> " ANSI_COLOR_RESET);
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
    }
    return;
}

void printCurrentDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf(ANSI_COLOR_MAGENTA "%s/\n", cwd);
}


void builtInCommands(char input[]){
    char *command[20];
    int i = 0;
    char *tmp;
    char line[100];

    strcpy(line, input);
    command[i] = strtok(line, " ");
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
    char line[100];
    strcpy(line, input);
    char *command = strtok(line, " ");
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
    char line[100];
    int running = 1;

    while(running) {
        printCurrentDir();
        inputString(line);
        storeCommand(line);
        if (!processOwnCommands(line))
            builtInCommands(line);
    }
    return(0);
}