#include "IOMngr.h"

FILE * source, * out;
char * currentLine;
int line, col;

int main() {
    if(openFiles("C:/Users/Duncan/Desktop/compsci/cs442/IOManager/sTest", "C:/Users/Duncan/Desktop/compsci/cs442/IOManager/lTest") == 0)
        return 1;
    char fChar;
    while(fChar != EOF) {
        fChar = getNextSourceChar();
    }

    writeInidcator(7);
    writeMessage("Missing semicolon");

    closeFiles();

    return 0;
}

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
        if(currentLine == NULL || currentLine == EOF)
            return EOF;

        col = 0;
        line++;
    }

    char sChar = currentLine[col];
    if(sChar == 0) // end of line has been reached, reset currentLine to NULL
        currentLine = NULL;
    else // puts the source char in the list file or stdout
        fputc(sChar, out);

    col++;
    return sChar;
}

void writeInidcator(int column) {
    fputc('\n', out);
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

int getCurrentLineNum() {
    return line;
}

int getCurrentColumnNum() {
    return col;
}