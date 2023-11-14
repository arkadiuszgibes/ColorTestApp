#ifndef _PARSE_H
#define _PARSE_H


#include <stdint.h>

#define PARSE_SUCCESS 0
#define PARSE_FAILED 1

uint8_t parse_hex_str_to_uint32(char * color_str, uint32_t  * hex_value);
uint8_t parse_octets_str_to_uint32(char * color_str, uint32_t * hex_value);


/**
 * Tries to get usable data from given line
 */
uint8_t parse_line(char * line, char * seq);


#endif // _PARSE_H
