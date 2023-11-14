#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cli.h"
#include "parse.h"
#include "colors.h"

/**
 **  CLI parameters defs
 */

#define PARAM_MODE_FULL "--mode"
#define PARAM_MODE_SHORT "-m"
#define PARAM_VALUES "..."

// by default mode not selected
uint8_t selected_mode = PARAM_MODE_null_v;

/**
 * Parse incomming commands and parameters
 */
uint8_t cli_process_commands(int argc, char *argv[]) {

	uint8_t mode_specified_flag = 0; // flag checks if mode was specified

	// start from second element
	for (uint8_t i = 1; i < argc; i++) {
		// check if this is -m or --mode command
		if (strstr(argv[i], PARAM_MODE_FULL)
				|| strstr(argv[i], PARAM_MODE_SHORT)) {

			mode_specified_flag = 1;

			// increase, so we get value next to the parameter
			i++;

			// check if didn't go so far
			if (i >= argc) {
				continue;
			}

			// assign selected mode;
			if (strcmp(argv[i], PARAM_MODE_mix) == 0) {
				selected_mode = PARAM_MODE_mix_v;

			} else if (strcmp(argv[i], PARAM_MODE_lowest) == 0) {
				selected_mode = PARAM_MODE_lowest_v;

			} else if (strcmp(argv[i], PARAM_MODE_highest) == 0) {
				selected_mode = PARAM_MODE_highest_v;

			} else if (strcmp(argv[i], PARAM_MODE_ms) == 0) {
				selected_mode = PARAM_MODE_ms_v;

			} else {
				selected_mode = PARAM_MODE_null_v;

				printf("Error, selected mode not supported\n");
				return 1;
			}

		} else if (strstr(argv[i], PARAM_VALUES)) {
			// allow for colors values only when there was a "..." before, otherwise
			// report an error!

			i++;

			uint32_t hex_value = 0;
			do {

				// call both parsers, if both fails, warn uresr about not parsable line
				if (	parse_hex_str_to_uint32(argv[i], &hex_value) 	== 	PARSE_FAILED &&
						parse_octets_str_to_uint32(argv[i], &hex_value) == 	PARSE_FAILED) {

					printf("Warning, could not parse color \"%s\", unknown type\n ", argv[i]);

				} else {
					// conversion success, add color to the list
					colors_add(hex_value);
				}

				i++;
				// do for all other parameters from the list
			} while (i < argc);

		}

		if (mode_specified_flag == 0) {
			printf("Warning, Mode parameter not specified, using default %s \n",
			PARAM_MODE_mix);
			return 1;
		}
	}

	return 0;
}

/**
 * Display single color in from of table
 */
void cli_display_single_color(const char *title, Colors_t *color) {

	/**
	 * Build table header
	 */

	printf("\n\nNew created color from selected mode: %s \n\n", title);
	printf("----------------------------------------------------------------------------------------------------------\n");

	// print header of table
	printf(
			"|    %3s    |    %3s    |    %3s    |    %3s    |    %9s    |    %6s    |    %4s    |    %4s    |\n",
			"R", "G", "B", "A", "HEX", "HUE", "SAT", "LTH");

	printf("-----------------------------------------------------------------------------------------------------------\n");

	/**
	 * fill row of table with data
	 */
	printf(
			"|    %3u    |    %3u    |    %3u    |    %3u    |    #%08x    |    %6.2f    |    %2.2f    |    %2.2f    |\n",
			color->color_rgb.R, color->color_rgb.G, color->color_rgb.B,
			color->color_rgb.A, color->color_hex, color->hsl.H, color->hsl.S,
			color->hsl.L);

	/**
	 * end of the table
	 */
	printf("-----------------------------------------------------------------------------------------------------------\n");
}

/**
 * Display table with all available data
 */
void cli_display_all_data(void) {

	uint16_t len = colors_get_color_array_lenght();

	/**
	 * Build table header
	 */

	printf("Color has ben succesfully modified\n\n");

	printf("----------------------------------------------------------------------------------------------------------\n");
	// print header of table
	printf("|    %3s    |    %3s    |    %3s    |    %3s    |    %9s    |    %6s    |    %4s    |    %4s    |\n",
			"R", "G", "B", "A", "HEX", "HUE", "SAT", "LTH");

	printf(
			"-----------------------------------------------------------------------------------------------------------\n");

	Colors_t *col1;

	// print table content
	for (uint16_t i = 0; i < len; i++) {
		col1 = colors_get_color(i);

		/**
		 * fill row of table with data
		 */
		printf(
				"|    %3u    |    %3u    |    %3u    |    %3u    |    #%08x    |    %6.2f    |    %2.2f    |    %2.2f    |\n",
				col1->color_rgb.R, col1->color_rgb.G, col1->color_rgb.B,
				col1->color_rgb.A, col1->color_hex, col1->hsl.H, col1->hsl.S,
				col1->hsl.L);
	}

	/**
	 * end of the table
	 */
	printf("-----------------------------------------------------------------------------------------------------------\n");
}

/**
 * Display simple hello message
 */
void cli_display_info(void) {

	printf("\n\n");

	printf(
			"#############################################################################################################################\n");
	printf(
			"################### This is a test application for mixing colors. by. arkadiusz.gibes@yahoo.com #############################\n");
	printf(
			"#############################################################################################################################\n\n");
	printf("Available arguments:\n");
	printf(
			"-m, --mode 		For selecting mode, mode accepts \"mix\", \"lowest\", \"highest\" and \"mix-saturate\" : \n\n");
	printf(
			"			\"mix\" - creates new color average of values from given colors, CLI and file\n");
	printf(
			"			\"lowest\" - creates new color from lowest from given colors, CLI and file\n");
	printf(
			"			\"highest\" - creates new color from highest from given colors, CLI and file\n");
	printf(
			"			\"mix-saturate\" - New saturation of last color based on average of all saturation, CLI and file\n\n\n");
	printf(
			"...			For additional input colors data. Accepted data formats [R,G,B,A] and HEX eg. #ff0000ff \n\n");

	printf(
			"#############################################################################################################################\n\n\n\n");

}

