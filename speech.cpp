// ==================================================================
// The speech class file.
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
// ==================================================================

#include "speech.h"
#include "globals.h"
#include "hardware.h"

//
// HINT: for this function and below: Check out the ULCD demo in the docs to see what
// uLCD methods you can use to draw graphics
// Link: https://os.mbed.com/users/4180_1/notebook/ulcd-144-g2-128-by-128-color-lcd/
//

////////////////////////////////////
// Static Function Declarations
////////////////////////////////////

/**
 * draw the speech bubble background.
 */
static void draw_speech_bubble();

/**
 * erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * draw a single line of the speech bubble.
 * @param line the text to display
 * @param which if TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);


void draw_speech_bubble()
{
    // draw a speech bubble at the bottom of the screen
    uLCD.filled_rectangle(0, 80, 127, 85, WHITE);       // top border
    uLCD.filled_rectangle(0, 114, 127, 117, WHITE);     // bottom border
    uLCD.filled_rectangle(0, 85, 2, 114, WHITE);        // left border
    uLCD.filled_rectangle(124, 86, 127, 117, WHITE);    // right border
    uLCD.filled_rectangle(3, 85, 124, 114, 0xf5f5f5);   // inside speech bubble
}

void erase_speech_bubble()
{
    // erase the speech bubble at the bottom of the screen
    uLCD.filled_rectangle(3, 80, 123, 115, 0);
}

void draw_speech_line(const char* line, int which)
{
    // set the location which line of text will go the uLCD
    if (which == TOP) { // top line
        uLCD.locate(1, 11);
    } else if (which == BOTTOM) { // bottom line
        uLCD.locate(1, 12);
    }
    
    // change text color and background
    uLCD.textbackground_color(0xf5f5f5);
    uLCD.color(0);

    // for each character in the text, write it to uLCD
    for (int i = 0; line[i] && i < 17; i++) // limit each line to 17 char
    {
        // write the character
        uLCD.printf("%c", (char)line[i]);
        // wait
        wait_ms(30); 
    }
}

void speech_bubble_wait()
{
    // keep waiting until the action button is pressed
    while (1) {
        if (!button1) {
            break;
        }
    }
}

////////////////////////////////////
// Drawing Function Declarations
////////////////////////////////////

void speech(const char* line1, const char* line2)
{
    // place the two lines into an array of lines
    // pass in to long_speech with the corresponding number of lines
    const char* array[] = {line1, line2};
    long_speech(array, 2);
}

void long_speech(const char* lines[], int n)
{
    // create a speech bubble
    draw_speech_bubble();

    // for each lines, display only two lines at a time
    for (int i = 0; i < n; ++i) {
        // if two lines are displayed, call the wait function
        if (i != 0 && i % 2 == 0) {
            speech_bubble_wait();
            draw_speech_bubble();   // draw bubble again
        }
        draw_speech_line(lines[i], i%2);
    }

    // erase the speech bubble
    erase_speech_bubble();
}
