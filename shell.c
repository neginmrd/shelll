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

int main() {
    int running = 1;
    char *line;
    while(running) {
        printCurrentDir();
        line = inputString(stdin, 10);
        free(line);
    }
    return(0);
}