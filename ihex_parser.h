#ifndef IHEX_PARSER
#define IHEX_PARSER

#include <stdint.h>
#include <stdbool.h>


typedef struct 
{
    uint8_t record_length;
	uint32_t start_address;
	uint16_t check_sum;
} StartLinearAddressRecord;

typedef struct
{
	uint8_t record_length;
	uint16_t address_field;
	uint16_t upper_address;
	uint16_t check_sum;
} ExtendedLinearAddressRecord;

typedef struct
{
	uint8_t record_length;
	uint16_t address_field;
	uint8_t data[16];
	uint16_t check_sum;
} DataRecord;

typedef struct
{
    StartLinearAddressRecord slar;
	ExtendedLinearAddressRecord elar;
	DataRecord dr;
} parsedLine;



parsedLine parseLine(char* line);
uint16_t getCurrentUpperAddress(void);

#endif