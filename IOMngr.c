#include "IOMngr.h"

FILE * source, * out;
char * currentLine;
int line, col;

int openFiles(char * sourceName, char * listingName) {
    source = fopen(sourceName, "r");
    if(source == NULL)  // file was unable to be created
        return 0;
    
    // source file successfuly opened
    // initializing file location identifiers
    // line == -1 means no lines have been read
    line = -1;
    col = 0;

    if(listingName != NULL && fopen(listingName, "r") != NULL) {
        out = fopen(listingName, "w");
    } else {
        out = stdout;
    }

    return 1;
}

void closeFiles() {
    if(source != NULL)
        fclose(source);

    if(out != NULL)
        fclose(out);
}

char getNextSourceChar() {
    if(source == NULL) // file doesn't exist / hasn't been opened yet
        return EOF;

    if(currentLine == NULL) {
        // currentLine is empty
        // get newline
        char buf[MAXLINE];
        currentLine = fgets(buf, MAXLINE, source);
        // end of file has been reached
        if(currentLine == NULL)
            return EOF;

        col = 0;
        line++;
        if(out != stdout) {
            fprintf(out, "%d. ", getCurrentLineNum());
        }
    }

    char sChar = currentLine[col];
    if(sChar == '\0') { 
        return EOF; 
    }

    if(sChar == '\n') { // end of line has been reached, reset currentLine to NULL
        currentLine = NULL;
    } 

    if(out != stdout) {
        fputc(sChar, out);
    }
    
    col++;
    return sChar;
}

void writeInidcator(int column) {
    for(int i = 0; i < column; i++) {
        fputc(' ', out);
    }
    fputc('^', out);
}

void writeMessage(char * message) {
    fputc('\n', out);
    fputs(message, out);
    fputc('\n', out);
}

int markError() {

}

int getCurrentLineNum() {
    return line + 1;
}

int getCurrentColumnNum() {
    return col;
}