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

int main(char *argv[], int argc) {
    // Create SymTab, open source file and listing file (if it exists)
    SymTab *table = createSymTab(17);
    if(openFiles("/home/compsci/cs442/h3/stest", NULL) == 0) { return 0; }
    char token;
    char *buf;
    int newToken, i;
    int index = 0; // index in current line
    int tlength = 0; // size of current token
    int errCols[MAXLINE+1] = { 0 }; // marks errors in the columns they occur in
    Attribute *attr;

    char testSpace[MAXLINE+1];

    // scan the full file
    do {
        token = getNextSourceChar();
        // load the next char from the source file into the token array
        tokenSpace[tlength] = token;
        strncpy(testSpace, tokenSpace, MAXLINE+1);
        // if the current line is empty, skip it
        if(tokenSpace[0] == '\n') { continue; }
        // if end of token encountered
        if(token == ' ' || token == '\n' || token == EOF) {
            int type;
            int first = index - tlength;
            // determining type of id
            if(tlength > 1 && tokenSpace[0] == '0' && tokenSpace[1] == 'x') {
                type = HEX;
            } else if(tokenSpace[0] >= 48 && tokenSpace[0] <= 57) {
                type = TEN;
            } else {
                type = ID;
            }
            // error checking the full token
            for(i = 0; i < tlength; i++) {
                if((tokenSpace[i] < 48 && tokenSpace[i] != ' ' && token != '\n') ||
                   (tokenSpace[i] > 57 && tokenSpace[i] < 65) ||
                   (tokenSpace[i] > 90 && tokenSpace[i] < 97) ||
                   (tokenSpace[i] > 122)) { // if the current char of the current token is illegal
                        // index - tlength = start of token in line
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
            if(errCols[first] == 0) {
                tokenSpace[tlength] = '\0';
                // enter token into the SymTab
                newToken = enterName(table, tokenSpace);
                // if the token is new, add a new attribute struct, else increment the count if it's an ID
                if(newToken) {
                    attr = (Attribute*) malloc(sizeof(Attribute));
                    if(type == ID) { attr->count = 1; }
                    attr->type = type;
                    setCurrentAttr(table, attr);
                } else {
                    attr = getCurrentAttr(table);
                    if(attr->type == ID)
                        attr->count++;
                }
            }
            // cleaning out token buffer
            // cleanTo(tokenSpace, tlength);
            for(i = 0; i < tlength; i++) {
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
            for(i = 0; i < index; i++) {
                // if an error column is odd, it's an illegal character
                if(errCols[i] % 2 == 1) {
                    writeInidcator(i);
                    writeMessage("Illegal character");
                    errCols[i]--;
                }
            }

            for(i = 0; i < index; i++) {
                // if an error column is not zero, it's a flag for an illegal token
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
    
    printf("\nToken\tType\tCount\n");
    do {
        attr = getCurrentAttr(table);
        if(attr->type == ID) {
            buf = "Id";
        } else if(attr->type == HEX) {
            buf = "Hex";
        } else {
            buf = "Ten";
        }
        printf("%s\t%s\t%d\n", getCurrentName(table), buf, attr->count);
        free(attr);
    } while(nextEntry(table));
    destroySymTab(table);
    
    return 0;
}