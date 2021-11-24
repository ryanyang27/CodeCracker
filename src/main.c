// Sample code for ECE 198

// Written by Bernie Roehl, August 2021

// This file contains code for a number of different examples.
// Each one is surrounded by an #ifdef ... #endif block inside of main().

// To run a particular example, you should remove the comment (//) in
// front of exactly ONE of the following lines:

// #define BUTTON_BLINK
// #define Algorithm_One
// #define LIGHT_SCHEDULER
// #define TIME_RAND
// #define KEYPAD
// #define KEYPAD_CONTROL
// #define SEVEN_SEGMENT
// #define KEYPAD_SEVEN_SEGMENT
// #define COLOR_LED
// #define ROTARY_ENCODER
// #define ANALOG
// #define PWM
// #define DEBUG

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include <string.h>
#include "ece198.h"
#include "cvector.h" // vector library
#include "output_helpers.h"

bool buttonIsOn()
{
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == false)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer
    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, 0); // on-board LED
    // InitializePin(GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, 0); // on-board LED

    // note: the on-board pushbutton is fine with the default values (no internal pull-up resistor
    // is required, since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the terminal (i.e. serial monitor) in VSCode)

    SerialSetup(9600);

    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)
    SerialPuts("Solve this riddle to continue in the escape room! \n");
    char riddle[] = "what is one plus one";
    char ans[] = "two";
    char *answer = strupr(ans);
    HAL_Delay(5000); // Startup program delay (arbitrary)

    /* OUTPUT RIDDLE IN MORSE CODE */
    flashMessage(riddle);

    /* SIGNAL FOR USER INPUT -- RAPID BLINK 30 TIMES */
    for (int i = 0; i < 30; i++)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);  // turn on LED
        HAL_Delay(100);                              // Pause
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); // turn off LED
        HAL_Delay(100);                              // Pause
    }

    /* RECORD BUTTON PRESS INTERVALS */
    bool lastButtonState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13); // false if on, true if off
    int begin = HAL_GetTick();                                   // start timer
    cvector_vector_type(int) inputIntervals = NULL;              // vector to record time intervals
    while (true)
    {
        bool currButtonState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
        if (currButtonState != lastButtonState)
        {
            if (buttonIsOn())
            {
                int elapsed = HAL_GetTick() - begin;        // how long the button was off for
                elapsed = -elapsed;                         // negate since button was off for this interval
                cvector_push_back(inputIntervals, elapsed); // append elapsed
            }
            else
            {
                int elapsed = HAL_GetTick() - begin;
                cvector_push_back(inputIntervals, elapsed); // how long the button was on for
            }
            begin = HAL_GetTick();
        }
        // Delay a little bit to avoid bouncing
        HAL_Delay(75);
        // save the current state as the last state, for next time through the loop
        lastButtonState = currButtonState;

        // Assume user has finished if no input for 10 units
        // so break while loop and return vector of ints
        if ((HAL_GetTick() - begin) > (10 * UNIT_LEN))
        {
            // first log is removed since it is the time interval until the button
            // was initially pressed
            cvector_erase(inputIntervals, 0);
            break;
        }
    }

    /* CONVERT TIME INTERVALS INTO MORSE CODE CHARACTERS*/
    cvector_vector_type(char) inputMorseChars = NULL;
    for (int i = 0; i < cvector_size(inputIntervals); i++)
    {
        int interval = inputIntervals[i];
        if (interval > 0 && interval < (2 * UNIT_LEN)) // If button is on for less than 2 units
        {
            // Append dot
            cvector_push_back(inputMorseChars, '.');
        }
        else if (interval >= (2 * UNIT_LEN)) // If button is on for more than 2 units
        {
            // Append dash
            cvector_push_back(inputMorseChars, '-');
        }
        else if (interval < (-2 * UNIT_LEN) && interval > (-6 * UNIT_LEN)) // If button is off for 2-6 units
        {
            // Append letter space
            cvector_push_back(inputMorseChars, ' ');
        }
        else if (interval <= (-6 * UNIT_LEN)) // If button is off for 6+ units then assume word space
        {
            // Append word space
            cvector_push_back(inputMorseChars, '/');
        }
        // Note that case of button being off for 0-2 units is not considered since that is
        // assumed to be space between morse code character inputs
    }
    cvector_free(inputIntervals); // deallocate unnecessary vector memory
    // Output user's input morse code
    SerialPuts("You entered: \n");
    for (int i = 0; i < cvector_size(inputMorseChars); i++)
    {
        SerialPutc(inputMorseChars[i]);
    }
    SerialPutc('\n');

    /* CONVERT ANSWER MESSAGE TO MORSE CHARS */
    cvector_vector_type(char) answerMorseChars = NULL;
    int i = 0;
    while (answer[i] != '\0')
    {
        if (answer[i] == ' ') // If space, then append back slash
        {
            cvector_push_back(answerMorseChars, '/'); // append word space
        }
        else
        {
            int letter_num = answer[i] - 65; // convert to [0, 25] bounds
            int j = 0;
            while (morse_code_letters[letter_num][j] != '\0') // append morse code sequence for this letter
            {
                cvector_push_back(answerMorseChars, morse_code_letters[letter_num][j]);
                j++;
            }
            cvector_push_back(answerMorseChars, ' '); // append morse code character space
        }
        i++;
    }
    cvector_pop_back(answerMorseChars); // Pop last redundant character space
    SerialPutc('\n');

    /* COMPARE INPUT MESSAGE TO RESPONSE */
    bool hasWon = true;
    for (int i = 0; i < cvector_size(answerMorseChars); i++)
    {
        // If any character is wrong, then player has lost the game
        if (answerMorseChars[i] != inputMorseChars[i])
        {
            hasWon = false;
        }
    }

    /* TELL USER IF HE/SHE HAS WON/LOST GAME */
    if (hasWon)
        flashMessage("You have won");
    else
        flashMessage("You have lost, please restart the game by pressing the black button.");

    // Deallocate memory
    cvector_free(inputMorseChars);
    cvector_free(answerMorseChars);
    return 0;
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}