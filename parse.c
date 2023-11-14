#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"

/**
 * Internal helper function for converting hex char to uint8 value
 */
static uint8_t hex_char_to_uint8(const char ch){
    /**
    ** ranges of our interest
    ** 48-57 numbers 0-9
    ** 65-70 capittals A-F
    ** 97-102 lower case a-f
    **/

    uint8_t val=0;

    /**
    **  check range of characters and simply calculate value based on
    **  character offset in ascii table.
    **/

    if( ch >= 48 &&  ch<=57 )
    {
        // we have 0-9 val = ascii - offset
        val = ch-48;

    }else if(ch >=65 && ch<=70 )
    {
        // we have A-F val = ascii - offset + digit ascii offset + 1
        val = ch-65+9+1;

    }else  if(ch >=97 && ch<=102 )
    {
        // we have a-f val = ascii - offset + digit ascii offset + 1
        val = ch-97+9+1;

    }else{
        return 16; // can't be more then 15 so indicate an error;
    }
    return val;
}


/**
 * Parse hex string to 32 bit unsigned word
 */
uint8_t parse_hex_str_to_uint32(char * hex_str, uint32_t  * hex_value){


    *hex_value = 0; // reinitialize

    uint16_t str_len = strlen(hex_str);

    // exclude dots
    if(!strstr(hex_str,",") )
    {
        // verify size
        if(str_len == 8) // we can't have longer hex then 4 octets
        {
            // iterate over string here
            for(uint16_t i = 0 ; i < str_len; i++)
            {
                //exclude end of line and line feed characters
                if(hex_str[i] != '\n' && hex_str[i] != 10){
                    uint8_t hex_octet = hex_char_to_uint8(hex_str[i]);
                    // can't be higher then 15
                    if(hex_octet < 16){
                        // make a full octet and fill up 32bits from most significant side
                        *hex_value = *hex_value | ( (hex_octet & 0xff) << ( 32 - ( ( i + 1 ) * 4 ) ) );
                        // #debug printf("Value: %8x \n", *hex_value);
                    }else{

                            printf("Error, unrecognized character:  %c \n", hex_str[i]);
                            return PARSE_FAILED;
                    }
                }
            }
        }else return PARSE_FAILED;
    } else return PARSE_FAILED;
    return PARSE_SUCCESS;
}


/**
 * Parse 4 octets of data seperated by seperator
 */
uint8_t parse_octets_str_to_uint32(char * octets_str, uint32_t * hex_value){

    uint8_t lenght = 0;
    lenght = strlen(octets_str);

    uint8_t sep_cnt=0;

    /**
    ** simple validation here, make sure
    ** that given string is parsable
    **/

    // check size of string
    if(lenght>7 && lenght < 16){
        // iterate
        for(uint8_t i = 0; i<=lenght ; i++){
            // check if we have digits and separators only
            if(isdigit(octets_str[i]) || octets_str[i] == ',')
            {
                // count separators
                if(octets_str[i] == ',')
                {
                    sep_cnt++;
                }
            }else
            {
                // ignore zeros on the end of the string
                if(octets_str[i]!=0)
                {

                    //printf("Warning, found not allowed characters in given string %s, \"%c\" is not a digit \n" , octets_str+i, (char)octets_str[i]);
                    return PARSE_FAILED;

                }
            }
        }

        if(sep_cnt!=3){

            //printf( "Warning, incorect number of separators '.', only 4 octets allowed - data ignored\n");
            return PARSE_FAILED;
        }
    }

    /**
    ** Parsing starts here
    **/

    char * str_segment;

    str_segment = strtok(octets_str,",");
    uint16_t int_seg[4];
    uint8_t segment_count = 0;

    // iterate the octets
    while (str_segment != NULL)
    {

        // check if characters are digit
        for (uint8_t i = 0; i < strlen(str_segment); i++)
        {
            if (!isdigit(str_segment[i]))
            {
                //printf("Octet contain not number character! %s\n", str_segment);
                return PARSE_FAILED;
            }
        }

        // convert to int an check if range is correct
        int_seg[segment_count] = atoi(str_segment);

        // verify range 0-255 after conversion
        if (int_seg[segment_count] < 0 || int_seg[segment_count] > 255)
        {
            //printf("Octet should be in range between 0-255 \n");
            return PARSE_FAILED;
        }

        // build 32 bit world with octets
        *hex_value = *hex_value | ( (int_seg[segment_count]) << ( 32 - ( (segment_count + 1 ) * 8 ) ) );

        segment_count++;
        //try to get new octet
        str_segment = strtok(NULL, ",");
    }

// everything good so return with 0 code.
  return PARSE_SUCCESS;
}




// function check if character is allowed
static uint8_t is_char_allowed(char ch){

    if( (ch >= 'a' && ch <= 'f') ||
        (ch >= 'A' && ch <= 'F') ||
        (ch >= '0' && ch <= '9') ||
        (ch == ',')){

            return 1;
        }
        else return 0;
}


/**
**  This function goes trough the line and looks for sequence of
**  allowed characters. 7-15 in lenght, 0-9, a-f, A-F and separator
**  it will pickup first match sequence, other characters will be ignored
**/
uint8_t parse_line(char * str, char * sequence){

    // indexes of begin of end of sequence in *str
    uint16_t begin_idx=0;
    uint16_t end_idx=0;

    // for sequence controls
    uint16_t sequence_cnt = 0;

    // loop ctrl
    uint16_t i = 0;

    // for dot validation
    uint8_t last_dot_index = 0;
    // for dot overflow control
    uint8_t dot_cnt = 0;

    // restarts sequence, clear sequence counter
    // and move sequence begin pointer
    inline void clear_sequence(){
        sequence_cnt = 0;
        begin_idx=i+1;
        dot_cnt=0;
    }

    // setup sequence end pointer to current position
    // and coppy sequence to the buffer
    inline void end_sequence(){
        end_idx = i;
        memcpy(sequence, str+begin_idx, end_idx-begin_idx);
    }

    while(1)
    {
        // check if character is allowed
        if(is_char_allowed( str[i] ))
        {
            // check if there are no dots next to each other
            if(str[i]== ','){

                // catch unwanted dots here
                // if dot is next to each other and at the begining
                // of the sequence
                if( ( (i - last_dot_index) < 2 && i > 1) || i == begin_idx ){
                    clear_sequence();
                    i++;
                    continue;
                }
                    last_dot_index = i;

                    // we already have 3 dots
                    // so ignore current one and finish sequence
                    if(dot_cnt == 3 ){
                        end_sequence();
                        break;
                    }

                // found valid dot so ++
                dot_cnt++;
            }

            // increase cnt becaue we have allowed char
            sequence_cnt++;

            // check for overflow
            // here we have one case for propper sequence
            if(sequence_cnt >= 16 || (dot_cnt == 0 && sequence_cnt >=9) ){
                end_sequence();
                break;
            }

        }else{ // found not allowed char

            /**
            **  When iteration fails, check if there is enought elements
            **  which could be use as a sequence
            **/
            if(sequence_cnt >= 7 && sequence_cnt <=  16){
                    // exclude dot from the end of the string.
                    // it's not parsable
                    if(str[i-1]!=','){
                        end_sequence();
                        break;
                    }else{
                        // there is a dot on the end
                        // reject current sequence
                        clear_sequence();
                    }
            }else{
                // we are not ready to end the sequence
                // move the begin index to current position and
                // clear sequence counter
                clear_sequence();
            }
        }
        // break loop when reach end of string
        if(str[i]==0)
            break;

        i++;
    }


    if(strlen( sequence) >= 7){
             return PARSE_SUCCESS;
    }

    return PARSE_FAILED;
}
