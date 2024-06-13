#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "TradeRecord.h"
#include "Converters.h"
#include "SplitString.h"

void convertCsvToXml(FILE* stream) {
    char line[1024];
    TradeRecord tradeObjects[1024];
    int lineCount = 0;
    int tradeCount = 0;
    int LotSize = 1;

    while (fgets(line, sizeof(line), stream)) {
        char* fields[3];
        int fieldCount = 0;
        char* token = strtok(line, ",");
        int tradeAmount;
        double tradePrice;

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

        if (!tryToExtractIntFromString(fields[1], &tradeAmount)) {
            fprintf(stderr, "WARN: Trade amount on line %d not a valid integer: '%s'\n", lineCount + 1, fields[1]);
        }

        if (!tryToExtractDoubleFromString(fields[2], &tradePrice)) {
            fprintf(stderr, "WARN: Trade price on line %d not a valid decimal: '%s'\n", lineCount + 1, fields[2]);
        }

        strncpy(tradeObjects[tradeCount].SourceCurrency, fields[0], 3);
        strncpy(tradeObjects[tradeCount].DestinationCurrency, fields[0] + 3, 3);
        tradeObjects[tradeCount].Lots = tradeAmount / LotSize;
        tradeObjects[tradeCount].Price = tradePrice;
        tradeCount++;
        lineCount++;
    }

    FILE* outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int index = 0; index < tradeCount; index++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", tradeObjects[index].SourceCurrency);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", tradeObjects[index].DestinationCurrency);
        fprintf(outFile, "\t\t<Lots>%d</Lots>\n", tradeObjects[index].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", tradeObjects[index].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>");
    fclose(outFile);
    printf("INFO: %d trades processed\n", tradeCount);
}
