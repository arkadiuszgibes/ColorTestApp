#ifndef _CLI_H
#define _CLI_H

#include "colors.h"

/**
**  text values of mode
*/
#define PARAM_MODE_mix "mix"
#define PARAM_MODE_lowest "lowest"
#define PARAM_MODE_highest "highest"
#define PARAM_MODE_ms "mix-saturate"

/**
**  enums;
*/
#define PARAM_MODE_null_v     0
#define PARAM_MODE_mix_v      1
#define PARAM_MODE_lowest_v   2
#define PARAM_MODE_highest_v  3
#define PARAM_MODE_ms_v       4

uint8_t cli_process_commands(int argc, char *argv[]);
void cli_display_all_data(void);
void cli_display_single_color(const char * title, Colors_t * color);
void cli_display_info(void);


#endif // _CLI_H

