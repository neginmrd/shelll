#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <setjmp.h>


#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_LINES_IN_FILE 500
#define MAX_CHARACTERS_IN_LINE 100
sigjmp_buf ctrlc_buf;

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
void discardComments(char lines[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE], int count) {
    // comment in middle of a line?
    char line[MAX_CHARACTERS_IN_LINE];
    for (int i = 0; i < count; i++) {
        strcpy(line, lines[i]);
        if(line[0] != '#')
            printf("%s\n", line);
    }
    return;
}

void removeWhiteSpace(char lines[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE], int count) {
    for (int i = 0; i < count; i++)
        for(int j = 0; lines[i][j]; j++)
            if(!(lines[i][j] == ' ' || lines[i][j] == '\n' || lines[i][j] == '\t'))
                printf("%c", lines[i][j]);
    printf("\n");
    return;
}

void freq(char lines[MAX_LINES_IN_FILE][MAX_CHARACTERS_IN_LINE], int count){
    char frequentWord[MAX_CHARACTERS_IN_LINE];
    char words [MAX_LINES_IN_FILE*MAX_CHARACTERS_IN_LINE][MAX_CHARACTERS_IN_LINE];
    int frequency;
    int maxCount = 0;
    int wordCount = 0;

    for(int i = 0; i < count; i++){
        for(int j=0; j < strlen(lines[i]); j++){
            for (int k=0; lines[i][j]!=' ' && j < strlen(lines[i]); k++)
                words[wordCount][k] = lines[i][j++];
            wordCount++;
        }
    }

    for (int i = 0; i < wordCount; i++){ 
        frequency = 1; 
        for (int j = i+1; j < wordCount; j++){
            if(!strcmp(words[i],words[j]))
                frequency++; 
        }
        if(frequency > maxCount){
            maxCount = frequency; 
            strcpy(frequentWord, words[i]); 
        }
    }
    printf("%s\n", frequentWord);
    return;
}

int isOwnCommand(char input[]){
    char ownCommands[6][15] = {"headlines", "freq", "noSpace", "noComment", "howmanylines", "head10"};
    int i = 0;
    while(i < 6){
        if(!strcmp(ownCommands[i], input)){
            // printf("found it in own commands.\n");
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
        fprintf(stderr, "Could not open file.\n"); 
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
    pid_t pid = fork();
    if (pid == 0){
        if (strcmp(command, "headlines") == 0) headlines(buffer, count);
        else if (strcmp(command, "head10") == 0) head(buffer, 10, count);
        else if (strcmp(command, "howmanylines") == 0) howmanylines(buffer, count);
        else if (strcmp(command, "discardComments") == 0) discardComments(buffer, count);
        else if (strcmp(command, "removeWhiteSpace") == 0) removeWhiteSpace(buffer, count);
        else if (strcmp(command, "freq") == 0) freq(buffer, count);
        exit(0);
    }
    else{
        wait(NULL);
        return(1);
    }
    return(1);
}

void handle_signals(int signo) {
    if (signo == SIGINT) {
        printf("\n");
        siglongjmp(ctrlc_buf, 1);
    }
}

int main() {
    char line[100];
    int running = 1;

    if (signal(SIGINT, handle_signals) == SIG_ERR) {
        printf("failed to register interrupts with kernel\n");
    } 
    while(running) {
        while (sigsetjmp(ctrlc_buf, 1 ) != 0 );
        printCurrentDir();
        inputString(line);
        storeCommand(line);
        if (!processOwnCommands(line))
            builtInCommands(line);
    }
    return(0);
}