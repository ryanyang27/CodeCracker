#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"

void flashSequence(char *sequence);
void flashDotOrDash(char dotOrDash);

const int UNIT_LEN = 1000;
//define the morse code for the alphabet and numbers
char *morse_code_letters[] = {
    ".-",   // A
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-",  // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--.."  // Z
};

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
