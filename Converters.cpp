#include <stdio.h>
#include <stdlib.h>
#include "Converters.h"

int tryToExtractIntFromString(const char* inputString, int* extractedInt) {
    char* splitedString;
    *extractedInt = strtol(inputString, &splitedString, 10);
    if (splitedString == inputString) {
        return 0;
    }
    return 1;
}

int tryToExtractDoubleFromString(const char* inputString, double* extractedDouble) {
    char* splitedString;
    *extractedDouble = strtod(inputString, &splitedString);
    if (splitedString == inputString) {
        return 0;
    }
    return 1;
}
