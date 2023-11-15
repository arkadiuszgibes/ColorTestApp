#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
#include "cli.h"
#include "colors.h"
#include "parse.h"
#include "fs.h"

#define FILE_NAME "colors.txt"

// user selected mode, origins from cli.c
extern uint8_t selected_mode;

int main(int argc, char *argv[]) {

	cli_display_info();

	// initialize colors base object
	colors_base_init();

	// process incomming commands and parameters
	cli_process_commands(argc, argv);

	// load data from file
	fs_load_file_data(FILE_NAME);

	// build indexed array with loaded data
	colors_build_index();

	// up to this moment there is no hsl data
	// lets create it!
	colors_update_hsl();

	// display data
	cli_display_all_data();

	// execute selected mode
	switch (selected_mode) {

	case PARAM_MODE_mix_v:
		colors_create_from_average();

		break;
	case PARAM_MODE_lowest_v:
		colors_create_from_lowest();
		break;

	case PARAM_MODE_highest_v:
		colors_create_from_highest();
		break;

	case PARAM_MODE_ms_v:

		colors_mix_saturation_last();

		// display table again
		cli_display_all_data();
		break;

	default:

		colors_create_from_average();
		break;
	}

	// clean allocated memmory
	colors_clean();

	exit(EXIT_SUCCESS);

	return 0;
}
