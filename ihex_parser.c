#include "ihex_parser.h"

#include <string.h>
#include <stdio.h>

static uint16_t current_upper_address = 0;

int strToInt(const char *str) {
    int result = 0;
    int i = 0;
    int sign = 1;

    if (str[i] == '-') {
        sign = -1;
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return sign * result;
}

uint8_t hex_to_number(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    return 0; 
}

int binaryToDecimal(const char* binary) { // taken from geeksforgeeks
    int dec = 0;
    
    int length = strlen(binary);
    int base = 1;
   
    for (int i = length - 1; i >= 0; i--) {
        if (binary[i] == '1') {
            dec += base;
        }
        
        base = base * 2;
    }
    
    return dec;
}


void cleanParser()
{
	current_upper_address = 0;
}

StartLinearAddressRecord parseStartLinearAddressRecord(char* line)
{
	StartLinearAddressRecord slar = {0};

	if (line[0] == ':')
	{
		slar.record_length = (hex_to_number(line[1]) << 4) | hex_to_number(line[2]);
		slar.check_sum = (hex_to_number(line[17]) << 4) | hex_to_number(line[18]);
		slar.start_address = (
			(hex_to_number(line[9]) << 28) 	|
			(hex_to_number(line[10]) << 24) |
			(hex_to_number(line[11]) << 20) |
			(hex_to_number(line[12]) << 16) |
			(hex_to_number(line[13]) << 12) |
			(hex_to_number(line[14]) << 8)  |
			(hex_to_number(line[15]) << 4)  |
			hex_to_number(line[16])
		);
	}

	return slar;
}

ExtendedLinearAddressRecord parseLinearAddressRecord_Line(char* line, uint8_t )
{
	ExtendedLinearAddressRecord ldr = {0};

	if (line[0] == ':')
	{
		ldr.record_length = (hex_to_number(line[1]) << 4) | hex_to_number(line[2]);
		ldr.address_field = (
			(hex_to_number(line[3]) << 12) |
			(hex_to_number(line[4]) << 8)  |
			(hex_to_number(line[5]) << 4)  |
			hex_to_number(line[6])
		);
		ldr.upper_address = (
			(hex_to_number(line[9]) << 12) |
			(hex_to_number(line[10]) << 8)  |
			(hex_to_number(line[11]) << 4)  |
			hex_to_number(line[12])
		);
		ldr.check_sum = (hex_to_number(line[13]) << 4) | hex_to_number(line[14]);

		current_upper_address = ldr.upper_address;
	} else 
	{
		return ldr;
	}
	return ldr;
}

DataRecord parseDataRecord(char* line)
{
	DataRecord dr = {0};

	if (line[0] == ':')
	{
		uint8_t sum = 0;
		dr.record_length = (hex_to_number(line[1]) << 4) | hex_to_number(line[2]);
		sum += dr.record_length;

		dr.address_field = (
			(hex_to_number(line[3]) << 12) |
			(hex_to_number(line[4]) << 8)  |
			(hex_to_number(line[5]) << 4)  |
			hex_to_number(line[6])
		);
		sum += (dr.address_field >> 8) & 0xFF;
		sum += dr.address_field & 0xFF;

		if (dr.record_length > 16) {return dr;}

		for (int index = 0; index < dr.record_length; index++)
		{
			int absolute_index = 9 + index*2;
			dr.data[index] = (hex_to_number(line[absolute_index]) << 4) | hex_to_number(line[absolute_index+1]);

			sum += dr.data[index];
		}
		dr.check_sum = (hex_to_number(line[9 + dr.record_length*2]) << 4) | hex_to_number(line[10 + dr.record_length*2]);
		sum += dr.check_sum;

		if ((sum & 0xFF) != 0) 
		{
			return dr;
		}

	} else 
	{
		return dr;
	}
	return dr;
} 


parsedLine parseLine(char* line)
{
	parsedLine result = {0};

	if (line[0] == ':')
	{
		switch (line[8])
		{
			case '0': // data record
				DataRecord dr = parseDataRecord(line);
			 	result.dr = dr;
				break;
			case '1': // EOF record. no return data needed
				cleanParser();
		
				break;
			case '4': // extended linear address record
				ExtendedLinearAddressRecord ldr = parseLinearAddressRecord_Line(line, 0);
				result.elar = ldr;
				break;
			case '5': // start linear address record
				StartLinearAddressRecord slar = parseStartLinearAddressRecord(line);
				result.slar = slar;
				break;
			default:
				printf("Unknown record type: %c\n", line[8]);
				break;
		}
	}

	return result;
}


uint16_t getCurrentUpperAddress(void)
{
	return current_upper_address;
}