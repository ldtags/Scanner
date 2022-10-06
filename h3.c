#include <stdio.h>
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

void cleanTo(char input[], char flag) {
    int i = 0;
    while(input[i] != flag) {
        input[i] = '\0';
        i++;
    }
}

void cleanTo(char input[], int length) {
    for(int i = 0; i < length; i++) {
        input[i] = '\0';
    }
}

int main(char *argv[], int argc) {
    // Create SymTab, open source file and listing file (if it exists)
    SymTab *table = createSymTab(17);
    if(openFiles(argv[0], argv[1]) == 0) { return 0; }
    char token;
    char *buf;
    int newToken, i;
    int index = 0, tlength = 0;
    int errCols[MAXLINE+1] = { 0 };
    Attribute *attr;

    // scan the full file
    while((token = getNextSourceChar()) != EOF) {

        // load the next char from the source file into the token array
        tokenSpace[tlength] = token;
        
        // full token encountered
        if(token == '\0' || token == '\n') {
            int type;

            // determining type of id
            if(tlength > 1 && tokenSpace[0] == '0' && tokenSpace[1] == 'x') {
                type = HEX;
            } else if(tokenSpace[0] >= 48 && tokenSpace[0] <= 57) {
                type = TEN;
            } else {
                type = ID;
            }

            for(i = 0; i < tlength; i++) {
                if((tokenSpace[i] < 48 && tokenSpace[i] != '\0' && token != '\n') ||
                   (tokenSpace[i] > 57 && tokenSpace[i] < 65) ||
                   (tokenSpace[i] > 90 && tokenSpace[i] < 97) ||
                   (tokenSpace[i] > 122)) {
                        errCols[index - tlength + i] += 1; // a one in an error column indicates an illegal character
                        if(tlength > 1) { 
                            errCols[index - tlength + i] += 2; // a two in an error column indicates an illegal token
                        } 
                } else if(type == TEN && tokenSpace[i] >= 65 && tokenSpace[i] <= 122) {
                    errCols[index - tlength] += 2;
                } else if(type == HEX && ((tokenSpace[i] > 70 && tokenSpace[i] < 91) || tokenSpace[i] > 102)) {
                    errCols[index - tlength] += 2;
                } else {
                    // token is legal
                    // enter token into the SymTab
                    newToken = enterName(table, tokenSpace);

                    // if the token is new, add an attribute struct, else increment the count if it's an ID
                    if(newToken) {
                        attr = (Attribute*) malloc(sizeof(Attribute));
                        attr->count = 1;
                        attr->type = type;
                        setCurrentAttr(table, attr);
                    } else {
                        attr = getCurrentAttr(table);
                        if(attr->type == ID)
                            attr->count++;
                    }
                }
            }

            // cleaning out token buffer
            cleanTo(tokenSpace, tlength);
            tlength = 0;
        } else {
            tlength++;
        }

        if(token == '\n') {
            int i;
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
            index++;
        }
    }

    // printing contents of the SymTab to stdout
    if(startIterator(table) == 0)
        return 0;
    
    do {
        printf("Token\tType\tCount\n");
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