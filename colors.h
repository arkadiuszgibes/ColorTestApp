#ifndef _COLORS_H
#define _COLORS_H

#include <stdint.h>

typedef struct Colors Colors_t;

// 4 bytes
typedef struct __RGB
{
    uint8_t A;
    uint8_t B;
    uint8_t G;
    uint8_t R;
}RGB_t;


typedef struct __HSL
{
    float H;
    float S;
    float L;
}HSL_t;


// total 24 bytes
typedef struct Colors
{
    // 4 bytes
    union
    {
        RGB_t color_rgb;
        uint32_t color_hex;
    };

    // 12 bytes
    HSL_t hsl;

    /**
    **  On 64bit machine there is a 8 byte allignement.
    **  we have total 24 so it's fine
    **/

    // 8 bytes;
    Colors_t * next;
}Colors_t;

// Data structure management functions
// linked list
void colors_base_init(void);
void colors_add(uint32_t color);

// indexed array
void colors_build_index(void);
void colors_clean();
Colors_t * colors_get_color(uint16_t index);
uint16_t colors_get_color_array_lenght();

// rgb2hal & hsl2rgb converters
void colors_rgb_to_hsl(Colors_t *color);
void colors_hsl_to_rgb(Colors_t *color);

// synch changes between rgb and hsl space
void colors_update_rgb(void);
void colors_update_hsl(void);

// create or modify colors
void colors_create_from_highest(void);
void colors_create_from_lowest(void);
void colors_create_from_average(void);
void colors_mix_saturation_last(void);

#endif // _COLORS_H
