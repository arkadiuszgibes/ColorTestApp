#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "colors.h"
#include "cli.h"

// linked list lenght
uint16_t colors_lenght;

// helper pointer to keep track of top of the list
Colors_t *colors_last;

// base object of colors linked list
Colors_t *colors;

// indexed list, its available only after
// colors_build_index envocation.
// access should be only via colors_get_color(index)
Colors_t **arr_colors_indexed;
uint16_t colors_indexed_lenght = 0;

/**
 ** Init function of colors, has to be called before we can add new elements to the list
 **/
void colors_base_init(void) {
	colors = (Colors_t*) calloc(1, sizeof(Colors_t));
	colors_last = colors;
}

/**
 **  Adds new elements to list
 **/
void colors_add(uint32_t color) {

	Colors_t *temp;
	// allocate mmemmory for object
	temp = (Colors_t*) calloc(1, sizeof(Colors_t));

	// assign new color
	temp->color_hex = color;

	/**
	 **  This is simple linked list implementation
	 **  we need to handle more then just one pointer to the next element
	 **  so there is no need for recuret iteration to go through the list when we need to add
	 **  next elements. That will speed up the process.
	 **/

	// set up child in last object
	colors_last->next = temp;
	// overide last object
	colors_last = temp;
	// increment lenght of structure
	colors_lenght++;
}

/**
 **  Build indexed array from the list
 **/
static void build_index(Colors_t *col) {
	// do it only onece
	if (colors_indexed_lenght == 0) {
		// allocate table of pointers to Colors_t of size colors->lenght
		arr_colors_indexed = (Colors_t**) calloc(colors_lenght + 1,
				sizeof(Colors_t*));
	}

	if (colors_indexed_lenght != 0) {
		// assign current pointer to the array
		arr_colors_indexed[colors_indexed_lenght - 1] = col;
	}

	// increment lenght of indexed list
	colors_indexed_lenght++;

	// end func if there is no more elements on the list
	if (col->next == NULL)
		return;

	// go to the next element
	build_index(col->next);
}

/**
 ** Wrapper function for hiding colors base object
 ** in this way there is no need to direct access from out of this file
 **/
void colors_build_index(void) {
	build_index(colors);

	// check if any elements in array
	// if not, we have no data
	if(colors_get_color_array_lenght() == 0){
		printf("Error, no data available\n\n");
		exit((EXIT_FAILURE));
	}
}

/**
 ** Clean after dynamic memmory allocation
 **/
void colors_clean(void) {
	for (uint16_t i = 0; i < colors_indexed_lenght; i++) {
		// remove all elements from indexed list
		free(arr_colors_indexed[i]);
	}
	// remove table of pointers
	free(arr_colors_indexed);
}

/**
 ** Return Color object with specified index from the array
 **/
Colors_t* colors_get_color(uint16_t index) {

	if (index < colors_indexed_lenght) {
		return arr_colors_indexed[index];
	} else {
		printf("Error, color out of range!");
		return NULL;
	}
}

/**
 **  Return lenght of array
 **/
uint16_t colors_get_color_array_lenght(void) {
	// return lenght -1, base element in array list which wasn't indexed
	return colors_indexed_lenght - 1;
}

/**
 ** Convert RGB to HSL space
 **/
void colors_rgb_to_hsl(Colors_t *color) {
	float r = color->color_rgb.R;
	float g = color->color_rgb.G;
	float b = color->color_rgb.B;

	// normalize rgb to the range 0-1
	r /= 255.0;
	g /= 255.0;
	b /= 255.0;

	// get the minimum and maximum values among RGB components
	float max_color = fmaxf(fmaxf(r, g), b);
	float min_color = fminf(fminf(r, g), b);

	HSL_t *hsl = &(color->hsl);

	// calculate the lightnes component
	hsl->L = (max_color + min_color) / 2.0;

	if (max_color == min_color) {
		// if max and min are equal, then we have gray
		hsl->H = 0.0;
		hsl->S = 0.0;
	} else {
		// calculate saturation component
		hsl->S =
				(hsl->L < 0.5) ?
						(max_color - min_color) / (max_color + min_color) :
						(max_color - min_color) / (2.0 - max_color - min_color);

		// calculate hue component
		if (max_color == r) {
			hsl->H = (g - b) / (max_color - min_color);
			if (g < b) {
				hsl->H += 6.0;
			}
		} else if (max_color == g) {
			hsl->H = (b - r) / (max_color - min_color) + 2.0;
		} else { // max_color == b
			hsl->H = (r - g) / (max_color - min_color) + 4.0;
		}
		// normalize hue to the range 0-1
		hsl->H /= 6.0;

		// convert hue to degrees
		hsl->H *= 360.0;
	}
}

/**
 ** Convert HSL to RGB space
 **/
void colors_hsl_to_rgb(Colors_t *color) {

	RGB_t *rgb = &(color->color_rgb);
	HSL_t *hsl = &(color->hsl);

	// calc c,x,m
	float c = (1 - fabs(2 * hsl->L - 1)) * hsl->S;
	float x = c * (1 - fabs(fmod(hsl->H / 60.0, 2) - 1));
	float m = hsl->L - c / 2.0;

	float r, g, b;

	// do math based on hue angle
	if (hsl->H >= 0 && hsl->H < 60) {
		r = c;
		g = x;
		b = 0;
	} else if (hsl->H >= 60 && hsl->H < 120) {
		r = x;
		g = c;
		b = 0;
	} else if (hsl->H >= 120 && hsl->H < 180) {
		r = 0;
		g = c;
		b = x;
	} else if (hsl->H >= 180 && hsl->H < 240) {
		r = 0;
		g = x;
		b = c;
	} else if (hsl->H >= 240 && hsl->H < 300) {
		r = x;
		g = 0;
		b = c;
	} else {
		r = c;
		g = 0;
		b = x;
	}

	// normalize to 255;
	rgb->R = (r + m) * 255.0;
	rgb->G = (g + m) * 255.0;
	rgb->B = (b + m) * 255.0;
}


void colors_update_hsl(void){


	uint16_t len = colors_get_color_array_lenght();

    Colors_t *color;

    // print table content
    for(uint16_t i = 0 ; i < len; i++){
    	// get colorg from table
        color = colors_get_color(i);
        // update hsl data based on RGB in col
        colors_rgb_to_hsl(color);
    }
}

void colors_update_rgb(void){

	uint16_t len = colors_get_color_array_lenght();

    Colors_t *color;

    // print table content
    for(uint16_t i = 0 ; i < len; i++){
    	// get colorg from table
        color = colors_get_color(i);
        // update rgb data based on HSL in col
        colors_hsl_to_rgb(color);
    }
}


void colors_mix_saturation_last(void) {

	/**
	 ** 1. Calculate average from all colors saturation
	 ** 2. Modify Saturation in HSL structure
	 ** 3. Call colors_hsl_to_rgb to update RGB struct
	 ** 3. Display results
	 **/

	uint16_t len = colors_get_color_array_lenght();

	float s = 0;


	// calc average of saturation
	for (uint16_t i = 0; i < colors_indexed_lenght - 1; i++) {
		s += arr_colors_indexed[i]->hsl.S;

	}
	s /= len;


	printf("\nAverage saturation of all components: %f\n\n", s);

	colors_last->hsl.S = s;
	// update rgb based on HSL
	colors_hsl_to_rgb(colors_last);

}

void colors_create_from_average(void) {

	Colors_t color;

	color.color_rgb.R  = 0;
	color.color_rgb.G  = 0;
	color.color_rgb.B  = 0;
	color.color_rgb.A  = 0;


	// summ
	for (uint16_t i = 0; i < colors_indexed_lenght - 1; i++) {
		color.color_rgb.R += arr_colors_indexed[i]->color_rgb.R;
		color.color_rgb.G += arr_colors_indexed[i]->color_rgb.G;
		color.color_rgb.B += arr_colors_indexed[i]->color_rgb.B;
		color.color_rgb.A += arr_colors_indexed[i]->color_rgb.A;
	}

	uint16_t len = colors_get_color_array_lenght();


	color.color_rgb.R  /= len;
	color.color_rgb.G  /= len;
	color.color_rgb.B  /= len;
	color.color_rgb.A  /= len;

	colors_rgb_to_hsl(&color);

	cli_display_single_color(PARAM_MODE_mix, &color);

}

void colors_create_from_highest(void) {

	Colors_t color;

	color.color_rgb.R  = 0;
	color.color_rgb.G  = 0;
	color.color_rgb.B  = 0;
	color.color_rgb.A  = 0;

	color.color_rgb.R = arr_colors_indexed[0]->color_rgb.R;
	color.color_rgb.G = arr_colors_indexed[0]->color_rgb.G;
	color.color_rgb.B = arr_colors_indexed[0]->color_rgb.B;
	color.color_rgb.A = arr_colors_indexed[0]->color_rgb.A;

	for (uint16_t i = 0; i < colors_indexed_lenght - 1; i++) {
		// select higher againg and again... for all rgba elements
		color.color_rgb.R = (color.color_rgb.R > arr_colors_indexed[i]->color_rgb.R) ?
				color.color_rgb.R : arr_colors_indexed[i]->color_rgb.R;
		color.color_rgb.G = (color.color_rgb.G > arr_colors_indexed[i]->color_rgb.G) ?
				color.color_rgb.G : arr_colors_indexed[i]->color_rgb.G;
		color.color_rgb.B = (color.color_rgb.B  > arr_colors_indexed[i]->color_rgb.B) ?
				color.color_rgb.B  : arr_colors_indexed[i]->color_rgb.B;
		color.color_rgb.A = (color.color_rgb.A > arr_colors_indexed[i]->color_rgb.A) ?
				color.color_rgb.A: arr_colors_indexed[i]->color_rgb.A;
	}


	colors_rgb_to_hsl(&color);
	cli_display_single_color(PARAM_MODE_highest, &color);
}

void colors_create_from_lowest(void) {

	Colors_t color;

	color.color_rgb.R  = 0;
	color.color_rgb.G  = 0;
	color.color_rgb.B  = 0;
	color.color_rgb.A  = 0;

	color.color_rgb.R = arr_colors_indexed[0]->color_rgb.R;
	color.color_rgb.G = arr_colors_indexed[0]->color_rgb.G;
	color.color_rgb.B = arr_colors_indexed[0]->color_rgb.B;
	color.color_rgb.A = arr_colors_indexed[0]->color_rgb.A;

	for (uint16_t i = 0; i < colors_indexed_lenght - 1; i++) {
		// select higher againg and again... for all rgba elements
		color.color_rgb.R = (color.color_rgb.R < arr_colors_indexed[i]->color_rgb.R) ?
				color.color_rgb.R : arr_colors_indexed[i]->color_rgb.R;
		color.color_rgb.G = (color.color_rgb.G < arr_colors_indexed[i]->color_rgb.G) ?
				color.color_rgb.G : arr_colors_indexed[i]->color_rgb.G;
		color.color_rgb.B = (color.color_rgb.B  < arr_colors_indexed[i]->color_rgb.B) ?
				color.color_rgb.B  : arr_colors_indexed[i]->color_rgb.B;
		color.color_rgb.A = (color.color_rgb.A < arr_colors_indexed[i]->color_rgb.A) ?
				color.color_rgb.A: arr_colors_indexed[i]->color_rgb.A;
	}


	colors_rgb_to_hsl(&color);

	cli_display_single_color(PARAM_MODE_lowest, &color);
}

