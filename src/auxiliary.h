#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"

void flashSequence(char *sequence);
void flashDotOrDash(char dotOrDash);
void Return_Question(char *const question_string);
bool isGameWon(int game_state);
char * EnterInput();
int CheckInput(char *input_string, char *solution_string, int attempts, int solution_string_length);

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

void Return_Question(char *const riddle) {
    // loop forever, blinking the LED
        HAL_Delay(5000); // Startup program delay (arbitrary)

        int i = 0;
        while (riddle[i] != '\0')
        {                          // loop through each letter
            SerialPutc(riddle[i]); // print letter to console

            if (riddle[i] == ' ')
            {
                // space char then delay 7 units
                HAL_Delay(7 * UNIT_LEN);
                i++;
                continue;
            }
            else
            {
                int letter_num = riddle[i] - 65;                        // convert to [0, 25] bounds
                char *m_code_sequence = morse_code_letters[letter_num]; // morse code for this letter
                flashSequence(m_code_sequence);
                i++;
            }
        }
    }
char * EnterInput() {
    // Enter input through button press
    char Sequence[10000];
    char *InputSequence = Sequence; // comfortable length
    int Input_Delay = 0; //Checking how long it's been since 
    int initial_time; // starting time
    int time_elapsed; // time without button pushes
    int final_time; // time that button has been pushed 
    int time; // time to determine morse code symbol
    while (true) {
        initial_time = HAL_GetTick(); //Initializing the starting time
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
        u_int32_t internaltimer = HAL_GetTick() - initial_time; // determining if the player is going to push the button
        if (internaltimer >= 3*UNIT_LEN) {
            ++Input_Delay;
            break;
        }   
    }
    time_elapsed = HAL_GetTick(); //wait for button press
    if (Input_Delay == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true); // turn on LED
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)))
        ; //Waiting for Button Release
        final_time = HAL_GetTick();
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); // turn off LED
    }
    if (Input_Delay == 0) { //User requests an input
        time = final_time - initial_time;
    } else { //Longer Pause
        time = initial_time - time_elapsed;
    }
        if (time < UNIT_LEN*2 && time >= 0) { // Shortest is Dot
            SerialPuts(".");
            strncat(InputSequence, ".", 1);
            Input_Delay = 0;
        } else if (time >= UNIT_LEN*2) { //Longest is Dash
            SerialPuts("-");
            strncat(InputSequence, "-", 1);
            Input_Delay = 0;
        } else if (time < 0) { // negative time is a space
            SerialPuts(" ");
            strncat(InputSequence, " ", 1);
        }
    }
    if (Input_Delay == 3) {
            for (int k; k < 4; ++k) {
        InputSequence[strlen(InputSequence)-k] = '\0';
        }
        return InputSequence;
    }
}
