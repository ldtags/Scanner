#include <stdio.h>
#include "SymTab.h"
#include "IOMngr.h"

typedef struct {
    char *type;
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
        tokenSpace[index] = token;
        
        // full token encountered
        if(token == '\0' || token == '\n') {
            int illegal = 0;
            char *type;

            if(tlength > 1 && tokenSpace[0] == '0' && tokenSpace[1] == 'x') {
                type = "Hex";
            } else if(tokenSpace[0] >= 48 && tokenSpace[0] <= 57) {
                type = "Ten";
            } else {
                type = "Id";
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
                } else if(type == "Ten" && tokenSpace[i] >= 65 && tokenSpace[i] <= 122) {
                    errCols[index - tlength] += 2;
                } else if(type == "Hex" && ((tokenSpace[i] > 70 && tokenSpace[i] < 91) || tokenSpace[i] > 102)) {
                    errCols[index - tlength] += 2;
                } else {
                    // enter token into the SymTab
                    newToken = enterName(table, tokenSpace);
        
                    // if the token is new, add an attribute struct, else increment the count if it's an ID
                    if(newToken) {
                        attr = (Attribute*) malloc(sizeof(Attribute));
                        attr->count = 1;
                        setCurrentAttr(table, attr);
                    } else {
                        attr = getCurrentAttr(table);
                        if(attr->type == "Id")
                            attr->count++;
                    }
                }
            }

            free(illegal);
            free(type);
            tlength = 0;
        } else {
            tlength++;
        }

        if(token == '\n') {
            i = 0;
            while(errCols[i] != -1) {
                writeInidcator(errCols[i]);
                writeMessage("Illegal character");
            }
            index = 0;
        } else {
            index++;
        }
    }
}