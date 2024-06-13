#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char SC[256]; // SourceCurrency
    char DC[256]; // DestibationCurrency
    float Lots;
    double Price;
} TR;


char** SplitString(const char* str, char delimiter) {
    int count = 0;
    const char* ptr = str;
    while (*ptr != '\0') {
        if (*ptr++ == delimiter) {
            count++;
        }
    }

    char** tokens = (char**)malloc(sizeof(char*) * (count + 2));
    int i = 0;
    ptr = str;
    char* token = (char*)malloc(strlen(str) + 1);
    int j = 0;
    while (*ptr != '\0') {
        if (*ptr == delimiter) {
            token[j] = '\0';
            tokens[i++] = strdup(token);
            j = 0;
        } else {
            token[j++] = *ptr;
        }
        ptr++;
    }
    token[j] = '\0';
    tokens[i++] = strdup(token);
    tokens[i] = NULL;
    free(token);
    return tokens;
}


int intGetFromString(const char* str, int* value) {
    char* endptr;
    *value = strtol(str, &endptr, 10);
    if (endptr == str) {
        return 0;
    }
    return 1;
}

int toDouble(const char* str, double* value) {
    char* endptr;
    *value = strtod(str, &endptr);
    if (endptr == str) {
        return 0;
    }
    return 1;
}

void Process(FILE* stream) {
    char line[1024];
    TR objects[1024];
    int lineCount = 0;
    int objectCount = 0;

    while (fgets(line, sizeof(line), stream)) {
        char* fields[3];
        int fieldCount = 0;
        char* token = strtok(line, ",");
        while (token != NULL) {
            fields[fieldCount++] = token;
            token = strtok(NULL, ",");
        }

        if (fieldCount != 3) {
            fprintf(stderr, "WARN: Line %d malformed. Only %d field(s) found.\n", lineCount + 1, fieldCount);
            continue;
        }

        if (strlen(fields[0]) != 6) {
            fprintf(stderr, "WARN: Trade currencies on line %d malformed: '%s'\n", lineCount + 1, fields[0]);
            continue;
        }

        int tam;
        if (!intGetFromString(fields[1], &tam)) {
            fprintf(stderr, "WARN: Trade amount on line %d not a valid integer: '%s'\n", lineCount + 1, fields[1]);
        }

        double tp;
        if (!toDouble(fields[2], &tp)) {
            fprintf(stderr, "WARN: Trade price on line %d not a valid decimal: '%s'\n", lineCount + 1, fields[2]);
        }

        strncpy(objects[objectCount].SC, fields[0], 3);
        strncpy(objects[objectCount].DC, fields[0] + 3, 3);
        objects[objectCount].Lots = tam / LotSize;
        objects[objectCount].Price = tp;
        objectCount++;
        lineCount++;
    }

    FILE* outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int i = 0; i < objectCount; i++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", objects[i].SC);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", objects[i].DC);
        fprintf(outFile, "\t\t<Lots>%d</Lots>\n", objects[i].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", objects[i].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>");
    fclose(outFile);
    printf("INFO: %d trades processed\n", objectCount);
}

