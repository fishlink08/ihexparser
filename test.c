#include "ihex_parser.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hex_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        parsedLine result = parseLine(line);
        if (result.dr.record_length > 0)
        {
            printf("Data Record - Length: %d, Address: 0x%04X\n", result.dr.record_length, result.dr.address_field);
        } else if (result.slar.record_length > 0) {
            printf("Start Linear Address Record - Length: %d, Start Address: 0x%08X\n", result.slar.record_length, result.slar.start_address);
        } else if (result.elar.record_length > 0) {
            printf("Extended Linear Address Record - Length: %d, Address: 0x%04X, Upper Address: 0x%04X\n", result.elar.record_length, result.elar.address_field, result.elar.upper_address);
        }
    }

    fclose(file);
    return 0;
}