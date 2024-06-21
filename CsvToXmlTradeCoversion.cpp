#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "TradeRecord.h"
#include "Converters.h"

// Function to tokenize a line into fields
bool tokenizeLine(char *line, char *fields[], int expectedFields) {
    int fieldCount = 0;
    char *token = strtok(line, ",");
    while (token != NULL) {
        fields[fieldCount++] = token;
        token = strtok(NULL, ",");
    }
    return fieldCount == expectedFields;
}

// Function to validate fields
bool validateFields(char *fields[], int lineCount) {
    if (strlen(fields[0]) != 6) {
        fprintf(stderr, "WARN: Trade currencies on line %d malformed: '%s'\n", lineCount, fields[0]);
        return false;
    }
    int tradeAmount;
    if (!tryToExtractIntFromString(fields[1], &tradeAmount)) {
        fprintf(stderr, "WARN: Trade amount on line %d not a valid integer: '%s'\n", lineCount, fields[1]);
        return false;
    }
    double tradePrice;
    if (!tryToExtractDoubleFromString(fields[2], &tradePrice)) {
        fprintf(stderr, "WARN: Trade price on line %d not a valid decimal: '%s'\n", lineCount, fields[2]);
        return false;
    }
    return true;
}

// Function to extract trade data from fields and populate TradeRecord
void extractTradeData(char *fields[], TradeRecord *trade, int LotSize) {
    strncpy(trade->SourceCurrency, fields[0], 3);
    trade->SourceCurrency[3] = '\0';
    strncpy(trade->DestinationCurrency, fields[0] + 3, 3);
    trade->DestinationCurrency[3] = '\0';
    tryToExtractIntFromString(fields[1], (int*)&trade->Lots);
    trade->Lots /= LotSize;
    tryToExtractDoubleFromString(fields[2], &trade->Price);
}

void writeTradesToXml(TradeRecord *tradeObjects, int tradeCount) {
    FILE *outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int i = 0; i < tradeCount; i++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", tradeObjects[i].SourceCurrency);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", tradeObjects[i].DestinationCurrency);
        fprintf(outFile, "\t\t<Lots>%d</Lots>\n", tradeObjects[i].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", tradeObjects[i].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>\n");
    fclose(outFile);
}

void convertCsvToXml(FILE *stream) {
    char line[1024];
    TradeRecord tradeObjects[1024];
    int lineCount = 0;
    int tradeCount = 0;
    const int LotSize = 1;

    while (fgets(line, sizeof(line), stream)) {
        char *fields[3];
        lineCount++;
        if (!tokenizeLine(line, fields, 3)) {
            fprintf(stderr, "WARN: Line %d malformed. Incorrect number of fields.\n", lineCount);
            continue;
        }
        if (!validateFields(fields, lineCount)) {
            continue;
        }
        extractTradeData(fields, &tradeObjects[tradeCount], LotSize);
        tradeCount++;
    }

    writeTradesToXml(tradeObjects, tradeCount);
    printf("INFO: %d trades processed\n", tradeCount);
}
