#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SplitString.h"

char** SplitString(const char* InputString, char delimiter) {
    int NumberOfDelimiters = 0;
    const char* inputStringIterPtr = InputString;
    while (*inputStringIterPtr != '\0') {
        if (*inputStringIterPtr++ == delimiter) {
            NumberOfDelimiters++;
        }
    }

    char** tokens = (char**)malloc(sizeof(char*) * (NumberOfDelimiters + 2));
    int indexOfTokens = 0;
    inputStringIterPtr = InputString;
    char* token = (char*)malloc(strlen(InputString) + 1);
    int indexOfToken = 0;
    while (*inputStringIterPtr != '\0') {
        if (*inputStringIterPtr == delimiter) {
            token[indexOfToken] = '\0';
            tokens[indexOfTokens++] = strdup(token);
            indexOfToken = 0;
        } else {
            token[indexOfToken++] = *inputStringIterPtr;
        }
        inputStringIterPtr++;
    }
    token[indexOfToken] = '\0';
    tokens[indexOfTokens++] = strdup(token);
    tokens[indexOfTokens] = NULL;
    free(token);
    return tokens;
}