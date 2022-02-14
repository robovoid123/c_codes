#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_STRING_SIZE 2

/* gets a line from stdin*/
char *
getLine(void)
{
    char *line;
    int size;
    int length;
    int c;

    size = INITIAL_STRING_SIZE; // starting string size is 2
    line = malloc(size); // we allocate some space for line
    length = 0;

    while ((c = getchar()) != EOF && c != '\n'){
        // if length of string is starting to get more
        // than the allocated size we realloce a multiple of 2
        // of the current size
        if (length >= size -1){
            size *= 2;
            line = realloc(line, size);
        }
        line[length++] = c;
    }
    // to specify the string has ended
    line[length] = '\0';
    return line;
}

int main(int argc, char **argv)
{
    int i;
    char* string;
    char n_str[10];

    puts("What is your name");
    string = getLine();
    strcpy(n_str, string);

    i = strlen(n_str);
    printf("%s  %d", n_str, i);

    // free(string);
    return 0;
    
}