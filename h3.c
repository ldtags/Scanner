#include <stdio.h>
#include <string.h>
#include "SymTab.h"
#include "IOMngr.h"
#define HEX 0
#define TEN 1
#define ID 2

typedef struct {
    int type;
    int count;
} Attribute;

char tokenSpace[MAXLINE+1];

int invalid(char c) {
    return (c < 48 && c != ' ' && c != '\n') || (c > 57 && c < 65) || (c > 90 && c < 97) || (c > 122);
}

int main(int argc, char *argv[]) {
    // Create SymTab, open source file and listing file (if it exists)
    SymTab *table = createSymTab(17);
    if(openFiles(argv[1], argv[2]) == 0) { return 0; }
    char token;
    int index = 0; // index in current line
    int tlength = 0; // size of current token
    int errCols[MAXLINE+1] = { 0 }; // marks errors in the columns they occur in
    Attribute *attr;

    // scan the full file
    do {
        token = getNextSourceChar();

        // load the next char from the source file into the token array
        tokenSpace[tlength] = token;

        // if the current line is empty, skip it
        if(tokenSpace[0] == ' ') {
            index++;
            continue; 
        }

        if(tokenSpace[0] == '\n') { continue; }

        // if end of token encountered
        if(token == ' ' || token == '\n' || token == EOF) {
            int type;
            int first = index - tlength;

            // determining id of token
            if(tlength > 1 && tokenSpace[0] == '0' && tokenSpace[1] == 'x') {
                type = HEX;
            } else if(tokenSpace[0] >= 48 && tokenSpace[0] <= 57) {
                type = TEN;
            } else {
                type = ID;
            }

            // error checking the full token for errors
            for(int i = 0; i < tlength; i++) {
                if(invalid(tokenSpace[i])) {
                    errCols[first + i] += 1; // a one in an error column indicates an illegal character
                    if(tlength > 1) {
                        errCols[first] += 2; // a two in an error column indicates an illegal token
                    }
                } else if(type == TEN && tokenSpace[i] >= 65 && tokenSpace[i] <= 122) {
                    errCols[first] += 2;
                } else if(type == HEX && i > 1 && ((tokenSpace[i] > 70 && tokenSpace[i] < 91) || tokenSpace[i] > 102)) {
                    errCols[first] += 2;
                } // else token is legal
            }

            // if the current token is legal
            if(errCols[first] == 0 && tokenSpace[0] != EOF) {
                tokenSpace[tlength] = '\0';
                // if the entered token is new, add a new attribute struct
                if(enterName(table, tokenSpace)) {
                    attr = (Attribute*) malloc(sizeof(Attribute));
                    if(type == ID) { attr->count = 1; }
                    attr->type = type;
                    setCurrentAttr(table, attr);
                } else {
                    // token exists, so increment count if token type is ID
                    attr = getCurrentAttr(table);
                    if(attr->type == ID)
                        attr->count++;
                }
            }
        
            // cleaning out token buffer
            for(int i = 0; i < tlength; i++) {
                tokenSpace[i] = '\0';
            }
            tlength = 0;
        } else {
            // there are more characters in the token
            tlength++;
        }

        // if end of line encountered
        if(token == '\n' || token == EOF) {
            // printing out errors
            for(int i = 0; i < index; i++) {
                // if an error column is odd on the first pass, it's a flag for an illegal character
                if(errCols[i] % 2 == 1) {
                    writeInidcator(i);
                    writeMessage("Illegal character");
                    errCols[i]--;
                }
            }
            for(int i = 0; i < index; i++) {
                // if an error column is not zero on the second pass, it's a flag for an illegal token
                if(errCols[i] != 0) {
                    writeInidcator(i);
                    writeMessage("Illegal token");
                    errCols[i] = 0;
                }
            }
            
            index = 0;
        } else {
            // there are more characters in the line
            index++;
        }
    } while(token != EOF);

    // printing contents of the SymTab to stdout
    if(startIterator(table) == 0)
        return 0;
    
    printf("Token\tType\tCount\n");
    do {
        attr = (Attribute *) getCurrentAttr(table);
        printf("%s\t%s", getCurrentName(table),
                               attr->type == HEX ? "Hex" : ( attr->type == TEN ? "Ten" : "Id" ));
        if(attr->type == ID) { printf("\t%d", attr->count); }
        fputc('\n', stdout);
        free(attr);
    } while(nextEntry(table));
    destroySymTab(table);
    return 0;
}