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
char *typeList[3] = {"Hex", "Ten", "Id"};

int invalid(char c) {
    return (c < 48 && c != ' ' && c != '\n') || (c > 57 && c < 65) || (c > 90 && c < 97) || (c > 122);
}

int main(int argc, char *argv[]) {
    // Create SymTab, open source file and listing file (if it exists)
    SymTab *table = createSymTab(17);
    if(openFiles("stest", argv[2]) == 0) { return 0; }
    char token;
    char tokenBuf[MAXLINE+1] = { 0 };
    int index = 0; // index in current line
    int tlength = 0; // size of current token
    int legal = 0; // determines if the line has any illegal tokens in it
    int errCols[MAXLINE+1] = { 0 }; // marks errors in the columns they occur in
    Attribute *attr;

    // scan the full file
    do {
        token = getNextSourceChar();

        // load the next char from the source file into the token array
        tokenSpace[index] = token;

        // if the current line is empty, skip it
        if(tokenSpace[0] == '\n') { continue; }

        // if end of token encountered
        if(token == ' ' || token == '\n' || token == EOF) {
            int type;
            int first = index - tlength;

            // determining type of id
            if(tlength > 1 && tokenSpace[first] == '0' && tokenSpace[first + 1] == 'x') {
                type = HEX;
            } else if(tokenSpace[first] >= 48 && tokenSpace[first] <= 57) {
                type = TEN;
            } else {
                type = ID;
            }

            // error checking the full token
            for(int i = first; i < index; i++) {
                if(invalid(tokenSpace[i])) {
                    errCols[i] += 1; // a one in an error column indicates an illegal character
                    if(tlength > 1) { 
                        errCols[first] += 2; // a two in an error column indicates an illegal token
                    } 
                } else if(type == TEN && tokenSpace[i] >= 65 && tokenSpace[i] <= 122) {
                    errCols[first] += 2;
                } else if(type == HEX && (i > first + 1) && ((tokenSpace[i] > 70 && tokenSpace[i] < 91) || tokenSpace[i] > 102)) {
                    errCols[first] += 2;
                } // else token is legal
            }


            // if the current token is legal
            if(errCols[first] == 0) {

                // loading the token into the token buffer
                for(int i = 0; i < tlength; i++) {
                    tokenBuf[i] = tokenSpace[first + i];
                }

                // if the entered token is new, add a new attribute struct
                if(enterName(table, tokenBuf)) {
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

                // cleaning out the token buffer
                for(int i = 0; i < tlength; i++) {
                    tokenBuf[i] = 0;
                }

            } else {
                // token is illegal
                legal = 1;
            }
        
            tlength = 0;
        } else {
            // there are more characters in the token
            tlength++;
        }

        // if end of line encountered
        if(token == '\n' || token == EOF) {
            if(legal == 1) { // line contains an illegal token
                if(tokenSpace[index] == EOF) {
                    tokenSpace[index] == '\n';
                }
                
                if(argc == 1) {
                    // if there is no listing file, print the error line to stdout
                    printf("%d. %s\n", getCurrentLineNum(), tokenSpace);
                }

                // printing out errors
                for(int i = 0; i < index; i++) {
                    // if an error column is odd, it's an illegal character
                    if(errCols[i] % 2 == 1) {
                        writeInidcator(i);
                        writeMessage("Illegal character");
                        errCols[i]--;
                    }
                }

                for(int i = 0; i < index; i++) {
                    // if an error column is not zero, it's a flag for an illegal token
                    if(errCols[i] != 0) {
                        writeInidcator(i);
                        writeMessage("Illegal token");
                        errCols[i] = 0;
                    }
                }
                legal = 0;
            }
            
            // cleaning out token buffer
            for(int i = 0; i < index; i++) {
                tokenSpace[i] = '\0';
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
        printf("%s\t%s\t%d\n", getCurrentName(table), typeList[attr->type], attr->count);
        free(attr);
    } while(nextEntry(table));
    destroySymTab(table);
    return 0;
}