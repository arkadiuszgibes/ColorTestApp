#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "parse.h"
#include "colors.h"
#include "fs.h"

void fs_process_file(FILE *fp) {

	char *line = NULL;
	size_t len = 0;
	size_t read;

	char *buffer = NULL;
	buffer = calloc(16, sizeof(char));

	while ((read = getline(&line, &len, fp)) != -1) {

		memset(buffer, 0, 16); // clear buffer

		if (parse_line(line, buffer) == PARSE_FAILED) {
			printf("Warning, found not parsable line : %s", line);

		}

		uint32_t hex_value = 0;

		// don't parse if not enought characters
		if (strlen(buffer) >= 7) {

			// call both parsers, if both fails, warn uresr about not parsable line
			if (parse_hex_str_to_uint32(buffer, &hex_value) 	== PARSE_FAILED &&
				parse_octets_str_to_uint32(buffer, &hex_value) 	== PARSE_FAILED	){
				printf("Warning, found not parsable line : %s", line);
			} else {
				// conversion success, add color to the list
				colors_add(hex_value);
			}
		}
	}

	// free allocated mem
	if (line)
		free(line);

	if(buffer)
		free(buffer);
}

void fs_load_file_data(char *file_name) {


	FILE *fp;
	// open files for read only
	fp = fopen(file_name, "r");
	if (fp == NULL) {
		printf("Error, could not open %s file\n", file_name);
		return;
	}

	fs_process_file(fp);

	fclose(fp);
}
