#include <stdbool.h>   // booleans, i.e. true and false
#include <stdio.h>     // sprintf() function
#include "auxiliary.h" // Morse code preliminaries

#include "ece198.h"

void flashDotOrDash(char dotOrDash);
void flashSequence(char *sequence);
void flashMessage(char *const msg);

void flashDotOrDash(char dotOrDash)
{
    // blink light on
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true); // turn on LED
    if (dotOrDash == '.')
    {
        HAL_Delay(UNIT_LEN);
    }
    else // must be a '-'
    {
        HAL_Delay(UNIT_LEN * 3);
    }
    // blink LED off
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); // turn off LED
    HAL_Delay(UNIT_LEN);                         // gap between flashes
}

void flashSequence(char *sequence)
{
    int i = 0;
    while (sequence[i] != NULL)
    {
        flashDotOrDash(sequence[i]);
        i++;
    }
    HAL_Delay(UNIT_LEN * 2); // gap between letters is 3 but subtracted by 1 because of automatic delay of 1 unit at end of each dot/dash
}

void flashMessage(char *in_msg)
{
    // in_msg is stored in a read-only memory segment hence
    // we need to copy string to new local_var
    char string[256];
    strcpy(string, in_msg);
    char *msg = strupr(string);

    int i = 0;
    while (msg[i] != '\0')
    {                       // loop through each letter
        SerialPutc(msg[i]); // print letter to console

        if (msg[i] == ' ')
        {
            // space char then delay 7 units
            HAL_Delay(7 * UNIT_LEN);
            i++;
            continue;
        }
        else
        {
            int letter_num = msg[i] - 65;                           // convert to [0, 25] bounds
            char *m_code_sequence = morse_code_letters[letter_num]; // morse code for this letter
            flashSequence(m_code_sequence);
            i++;
        }
    }
    SerialPutc('\n');
}