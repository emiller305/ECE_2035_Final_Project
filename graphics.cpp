// ==================================================================
// The graphics class file
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
// ==================================================================

#include "graphics.h"
#include "globals.h"



///////////////////////////////////////////
// Drawing Images Based on Characters
///////////////////////////////////////////

// additional color definitions
#define YELLOW 0xFFFF00
#define BROWN  0xD2691E
#define DIRT   BROWN
#define PURPLE 0xA020F0
#define DGREEN 0x009E60
#define LGREEN 0xAFE1AF
// You can define more hex colors here

/**
 * function to draw images based on characters
 * takes in an image array and changes color
 * labeled by a character to a hex value (ex: 'Y' -> 0xFFF00)
**/
void draw_img(int u, int v, const char* img)
{
    int colors[11*11];
    for (int i = 0; i < 11*11; i++)
    {
        // you can add more characters by defining their hex values above
        if (img[i] == 'R') colors[i] = RED;
        else if (img[i] == 'Y') colors[i] = YELLOW;
        else if (img[i] == 'G') colors[i] = GREEN;
        else if (img[i] == 'D') colors[i] = DIRT;
        else if (img[i] == '5') colors[i] = LGREY;
        else if (img[i] == '3') colors[i] = DGREY;
        else if (img[i] == 'B') colors[i] = BLUE;
        else if (img[i] == 'P') colors[i] = PURPLE;
        else if (img[i] == '4') colors[i] = DGREEN;
        else if (img[i] == 'L') colors[i] = LGREEN;
        else if (img[i] == 'W') colors[i] = WHITE;
        else colors[i] = BLACK;
    }
    uLCD.BLIT(u, v, 11, 11, colors);
    wait_us(250); // Recovery time!
}


///////////////////////////////////////////////////
// Simple Drawing of Objects Using uLCD Methods
///////////////////////////////////////////////////

void draw_nothing(int u, int v)
{
    uLCD.filled_rectangle(u, v, u+10, v+10, BLACK);
}

void draw_player(int u, int v, int key, bool gift)
{
    if (key) // player has the key!
    {
        // body
        uLCD.filled_circle(u, v, 2, GREEN);
        // head
        uLCD.filled_rectangle(u-2, v+2, u+2, v+4, GREEN);
        // hat
        if (!gift) uLCD.triangle(u-3, v-2, u, v-6, u+3, v-2, YELLOW);
        if (gift) {
            uLCD.filled_rectangle(u-2, v-2, u+2, v-10, WHITE); // top of hat
            uLCD.filled_rectangle(u-4, v-1, u+4, v-2, WHITE); // bottom of hat
        }
        // arms & legs
        uLCD.line(u-2, v+3, u-4, v+3, GREEN);
        uLCD.line(u+2, v+3, u+4, v+3, GREEN);
        uLCD.line(u-1, v+4, u-1, v+7, GREEN);
        uLCD.line(u+1, v+4, u+1, v+7, GREEN);
    }
    else
    {
        // body
        uLCD.filled_circle(u, v, 2, BLUE);
        // head
        uLCD.filled_rectangle(u-2, v+2, u+2, v+4, BLUE);
        // hat
        if (!gift) uLCD.triangle(u-3, v-2, u, v-6, u+3, v-2, YELLOW);
        if (gift) {
            uLCD.filled_rectangle(u-2, v-2, u+2, v-10, WHITE); // top of hat
            uLCD.filled_rectangle(u-4, v-1, u+4, v-2, WHITE); // bottom of hat
        }
        // arms & legs
        uLCD.line(u-2, v+3, u-4, v+3, BLUE);
        uLCD.line(u+2, v+3, u+4, v+3, BLUE);
        uLCD.line(u-1, v+4, u-1, v+7, BLUE);
        uLCD.line(u+1, v+4, u+1, v+7, BLUE);
    }
}


void draw_wall(int u, int v)
{
    uLCD.filled_rectangle(u, v, u+10, v+10, 0x808080); // grey walls
}

void draw_door(int u, int v)
{
    draw_nothing(u,v);
    uLCD.line(u, v+6, u+11, v+6, 0xFFFF00);
}

void draw_new_door(int u, int v)
{
    draw_nothing(u,v);
    uLCD.line(u, v+6, u+5, v+6, GREEN);
    uLCD.line(u+6, v+6, u+5, v+6, GREEN);
}

/**
 * draw the upper status bar.
 */
void draw_upper_status(int x, int y, bool k)
{
    uLCD.locate(0,0);
    uLCD.text_height(1);
    uLCD.text_width(1);
    uLCD.set_font(FONT_7X8);
    uLCD.filled_rectangle(0, 0, 127, 17, 0xf6f6f6);
    uLCD.textbackground_color(0xf6f6f6);
    uLCD.color(0);
    uLCD.printf(" Player:(%i,%i)\n Has Key: %s   \n", x, y, k ? "true" : "false");
}

/**
 * draw the lower status bar.
 */ 
void draw_lower_status(int h)
{
    uLCD.locate(0,14);
    uLCD.text_height(1);
    uLCD.text_width(1);
    uLCD.filled_rectangle(0, 110, 127, 120, 0xf6f6f6);
    uLCD.textbackground_color(0xf6f6f6);
    uLCD.filled_rectangle(60, 112, 70+h, 117, 0x4CBB17);
    uLCD.rectangle(59, 112, 120, 117, BLACK);
    uLCD.color(0);
    uLCD.printf(" Health:");
}

/**
 * draw the border for the map.
 */
void draw_border()
{
    uLCD.filled_rectangle(0,     0, 127,  3,  0xf6f6f6); // top
    uLCD.filled_rectangle(0,    13,   2, 114, 0xf6f6f6); // left
    uLCD.filled_rectangle(0,   114, 127, 117, 0xf6f6f6); // bottom
    uLCD.filled_rectangle(124,  14, 127, 117, 0xf6f6f6); // right
}

/**
 * draw the screen for game configuration.
*/
void draw_config() {
    // display control configuration page
    uLCD.cls();
    uLCD.filled_rectangle(0, 0, 130, 130, 0xf6f6f6);
    uLCD.textbackground_color(0xf6f6f6);
    uLCD.set_font(FONT_8X12);
    uLCD.text_height(1);
    uLCD.text_width(1);
    uLCD.color(BLACK);
    uLCD.locate(2,1);
    uLCD.printf("Game Controls");
    uLCD.line(1, 20, 125, 20, BLUE);
    uLCD.locate(1, 3);
    uLCD.printf("Nav Switch:\n UP -> North\n DOWN -> South\n LEFT -> West\n RIGHT -> East\n CENTER -> Run");
    uLCD.line(1, 80, 125, 80, BLUE);
    uLCD.locate(0, 11);
    uLCD.printf(" Buttons:        B1 -> Action");
    uLCD.locate(0, 13);
    uLCD.printf(" B2 -> Inventory B3 -> Special");
    // borders
    uLCD.line(1, 1, 125, 1, BLUE); // top
    uLCD.line(1, 125, 125, 125, BLUE); // bottom
    uLCD.line(1, 1, 1, 125, BLUE); // right
    uLCD.line(125, 1, 125, 125, BLUE); // left
}

/**
 * draw the start up screen.
*/
void draw_start_up() {
    // display start page
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    uLCD.set_font(FONT_8X12);
    uLCD.text_height(1);
    uLCD.locate(2, 2);
    uLCD.color(WHITE);
    uLCD.printf("THE LEGEND OF");
    uLCD.locate(2, 4);
    uLCD.set_font(FONT_5X7);
    uLCD.color(RED);
    uLCD.text_height(2);
    uLCD.text_width(2);
    uLCD.printf("BURDELL");
    uLCD.set_font(FONT_8X12);
    uLCD.text_height(1);
    uLCD.text_width(1);
    uLCD.locate(4, 7);
    uLCD.color(WHITE);
    uLCD.printf("-MMXXIII-");
    uLCD.color(0xffffe0);
    uLCD.text_height(1);
    uLCD.text_width(1);
    uLCD.locate(2, 11);
    uLCD.printf("Press action\n button to start");
    // borders
    uLCD.line(1, 1, 125, 1, BLUE); // top
    uLCD.line(1, 125, 125, 125, BLUE); // bottom
    uLCD.line(1, 1, 1, 125, BLUE); // right
    uLCD.line(125, 1, 125, 125, BLUE); // left
}

//////////////////////////////////////////////////////
// Sprite Drawing of Objects Using draw_img Function
//////////////////////////////////////////////////////

void draw_plant(int u, int v)
{
    const char* img = 
        "           "
        " GGGGGGGG  "
        " GGGGGGGGG "
        "GGGGGGGGGGG"
        "GGGGGGGGGGG"
        " GGGGGGGG  "
        "    DD     "
        "    DD     "
        "    DD     "
        "   DDDDD   "
        "  D  D  D  ";
    draw_img(u, v, img);
}


void draw_npc(int u, int v)
{
    const char* img = 
        "     R     "
        "    RRR    "
        "   RRRRR   "
        "  RRRRRRR  "
        " RRRRYRRRR "
        "RRRRYYYRRRR"
        " RRRRYRRRR "
        "  RRRRRRR  "
        "   RRRRR   "
        "    RRR    "
        "     R     ";
    draw_img(u, v, img);
}

void draw_stairs(int u, int v)
{
    const char* img =
        "        333"
        "        353"
        "      33333"
        "      35553"
        "    3333333"
        "    3555553"
        "  333333333"
        "  355555553"
        "33333333333"
        "35555555553"
        "33333333333";
    draw_img(u, v, img);
}



////////////////////////////////////////////////
// Examples of Piskel Sprite C Array Export
////////////////////////////////////////////////

void draw_buzz(int u, int v)
{

int new_piskel_data[121] = {
0x00000000, 0x00000000, 0x00000000, 0xff58110c, 0xff58110c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff58110c, 0x00000000, 0x00000000, 0xff58110c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xffffff00, 0xffffffff, 0xff0000ff, 0xff606060, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xffffff00, 0xffffff00, 0xff0000ff, 0xffffffff, 0xff0000ff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 
0x00000000, 0xffffff00, 0xffffff00, 0xffffff00, 0xff0000ff, 0xff0000ff, 0xff606060, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 
0x00000000, 0x00000000, 0xffffff00, 0xffffffff, 0xffffffff, 0xffffff00, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xffffff00, 0xffffff00, 0xff58110c, 0xff58110c, 0xff137bff, 0xffffffff, 0x00000000, 0x00000000, 
0xff137bff, 0x00000000, 0xff137bff, 0xff58110c, 0xff58110c, 0xff58110c, 0xffffff00, 0xff58110c, 0xff137bff, 0x00000000, 0x00000000, 
0x00000000, 0xff137bff, 0xff137bff, 0x00000000, 0x00000000, 0xffffff00, 0xff137bff, 0xff137bff, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xffffff00, 0xffffff00, 0xff58110c, 0xff58110c, 0xff58110c, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff58110c, 0xffffff00, 0xffffff00, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

   uLCD.BLIT(u,v, 11,11, new_piskel_data);
}

void draw_slain_buzz(int u, int v)
{

int new_piskel_data[121] = {
0x00000000, 0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xffcccb,   0x00000000, 0x00000000, 0xffcccb,   0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 
0x00000000, 0xffffff00, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 
0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,    0xffcccb,  0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 
0xffcccb,   0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 
0x00000000, 0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xffffff00, 0xffcccb,   0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffcccb,   0xffcccb,   0xffcccb,   0x00000000, 0x00000000, 0x00000000, 0x00000000
};

   uLCD.BLIT(u,v, 11,11, new_piskel_data);
}


void draw_water(int u, int v)
{


int new_piskel_data[121] = {

0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff0101c4, 0xff0101c4, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff0101c4, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0xff0101c4, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0xff0101c4, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0xff0101c4, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0x00000000, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0x00000000, 0xff0101c4, 0xff0101c4, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff7c7cff, 0xff0101c4, 0xff0101c4, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0xff0101c4, 0x00000000, 0x00000000, 0x00000000

};
   uLCD.BLIT(u,v, 11,11, new_piskel_data);
}

void draw_fire(int u, int v)
{

int new_piskel_data[121] = {

0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0xffff0009, 0xffff0009, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0xffff0009, 0xffff0009, 0xffff0009, 0xffff0009, 0x00000000, 0xffff0009, 0xffff0009, 0xffff0009, 0x00000000, 0x00000000, 0x00000000, 
0xffff0009, 0xffff0009, 0xffdeb200, 0xffff0009, 0x00000000, 0xffff0009, 0xffdeb200, 0xffff0009, 0xffff0009, 0x00000000, 0x00000000, 
0x00000000, 0xffff0009, 0xffdeb200, 0xffdeb200, 0xffff0009, 0xffdeb200, 0xffdeb200, 0xffff0009, 0xffff0009, 0x00000000, 0x00000000, 
0x00000000, 0xffff0009, 0xffdeb200, 0xffdeb200, 0xffdeb200, 0xffdeb200, 0xffdeb200, 0xffff0009, 0xffff0009, 0xffff0009, 0x00000000, 
0x00000000, 0xffff0009, 0xffde4600, 0xffde4600, 0xffdeb200, 0xffdeb200, 0xffdeb200, 0xffff0009, 0xffff0009, 0xffff0009, 0xffff0009, 
0xffff0009, 0xffff0009, 0xffde4600, 0xffde4600, 0xffde4600, 0xffde4600, 0xffde4600, 0xffff0009, 0xffde4600, 0xffde4600, 0xffff0009, 
0xffff0009, 0xffb30007, 0xffb30007, 0xffb30007, 0xffde4600, 0xffde4600, 0xffde4600, 0xffde4600, 0xffde4600, 0xffff0009, 0xffff0009, 
0xffff0009, 0xffb30007, 0xffb30007, 0xffb30007, 0xffde4600, 0xffde4600, 0xffb30007, 0xffb30007, 0xffb30007, 0xffff0009, 0x00000000, 
0xffff0009, 0xffff0009, 0xffff0009, 0xffb30007, 0xffb30007, 0xffb30007, 0xffb30007, 0xffb30007, 0xffff0009, 0xffff0009, 0xffff0009

};
   uLCD.BLIT(u,v, 11,11, new_piskel_data);
}

void draw_earth(int u, int v)
{

int new_piskel_data[121] = {

0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xffffffff, 0xff00659e, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 
0x00000000, 0xffffffff, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0xffffffff, 0x00000000, 
0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0x00000000, 
0xffffffff, 0xffffffff, 0xff00659e, 0xffffffff, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0x00000000, 
0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0x00000000, 
0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0xffffffff, 0xffffffff, 
0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0xff00659e, 0xff00659e, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e, 
0xff00659e, 0xff00659e, 0xff00659e, 0xff00659e, 0xffffffff, 0xff00659e, 0xffffffff, 0xffffffff, 0xff00659e, 0xff00659e, 0xff00659e

};
   uLCD.BLIT(u,v, 11,11, new_piskel_data);
}





//////////////////////////////
// Other Sprites
//////////////////////////////

void draw_cave1(int u, int v)
{
    const char* img =
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333";
    draw_img(u, v, img);
}
void draw_cave2(int u, int v)
{
    const char* img =
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333";
    draw_img(u, v, img);
}
void draw_cave3(int u, int v)
{
    const char* img =
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333000"
        "33333333000"
        "33333333000";
    draw_img(u, v, img);
}
void draw_cave4(int u, int v)
{
    const char* img =
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333"
        "33333333333";
    draw_img(u, v, img);
}


void draw_mud(int u, int v)
{
   const char* img = 
        "DDDDDDDDDDD"
        "DDD3333DD3D"
        "D33D33D33DD"
        "D3DDD33D33D"
        "DD333D333DD"
        "D33D33DDDD"
        "DDD333D333D"
        "DD3DDD3DD3D"
        "D3D333D33DD"
        "DDDDD33DDDD"
        "DDDDDDDDDDD";
    draw_img(u, v, img);
}

void draw_wreck(int u, int v) {
    const char* img = 
        "00YYYYYYY00"
        "500YY0YY005"
        "Y500Y0Y005Y"
        "YY50Y0Y05YY"
        "YY500YY005Y"
        "Y050Y5Y050Y"
        "050Y535Y050"
        "50YY535YY05"
        "50Y53535Y05"
        "05YY5535YY5"
        "050Y535Y050"
        "050Y535Y050"
        "05335353350"
        "05335353350"
        "03300000330"
        "03300000330";
    draw_img(u, v, img);
}

void draw_pebble(int u, int v) 
{
    const char* img = 
        "     5     "
        "     5     "
        "   53335   "
        "   53335   "
        "  5353535  "
        "  5353535  "
        " 553335333 "
        " 553335333 "
        "53533353355"
        "55555335555"
        "55555555555";
    draw_img(u, v, img);
}

void draw_power_up(int u, int v)
{
    const char* img = 
        "GGGBBBBBGGG"
        " GGGBBBGGG "
        "  GGGBGGG  "
        "   GGBGG   "
        "    GBG    "
        "     B     "
        "           "
        "           "
        "           "
        "           "
        "           ";
    draw_img(u, v, img);
}

void draw_gift_box(int u, int v)
{
    const char* img = 
        "P         P"
        "PP       PP"
        "PPP     PPP"
        "PPPPP PPPPP"
        "5P5P5P5P5P5"
        "5PPYYYYYPP5"
        "5YPPYYYPPY5"
        "5YYPPYPPYY5"
        "5YYPPYPPYY5"
        "5YYYPPPPYY5"
        "55555555555";
    draw_img(u, v, img);
}

void draw_bush(int u, int v)
{
    const char* img = 
        " 44P44R44  "
        " 44R44R44  "
        " P444P4444 "
        "44R4B444R44"
        "4B444B4444R"
        "B4R44R44R44"
        " 444444R44 "
        " 444444444 "
        "     D     "
        "     D     "
        "           ";
    draw_img(u, v, img);
}

void draw_hole(int u, int v)
{
    const char* img = 
        "    555    "
        "  55DDD55  "
        " 55DDDDD55 "
        " 55DDDDD55 "
        " 55DDDDD55 "
        "  55DDD55  "
        "    555    "
        "           "
        "           "
        "           "
        "           ";
    draw_img(u, v, img);
}

void draw_secret_entrance(int u, int v)
{
    const char* img = 
        "44444444444"
        "4LLLLLLLLL4"
        "4LDDDDDDDL4"
        "4LDDDDDDDL4"
        "4LDDDDDDDL4"
        "4LDDDDD00L4"
        "4LDDDDD00L4"
        "4LDDDDDDDL4"
        "4LDDDDDDDL4"
        "4LLLLLLLLL4"
        "44444444444";
    draw_img(u, v, img);
}

void draw_secret_stairs(int u, int v)
{
    const char* img =
        "        444"
        "        4G4"
        "      44444"
        "      4GGG4"
        "    4444444"
        "    4GGGGG4"
        "  444444444"
        "  4GGGGGGG4"
        "44444444444"
        "4GGGGGGGGG4"
        "GGGGGGGGGGG";
    draw_img(u, v, img);
}

void draw_mushroom(int u, int v)
{
    const char* img = 
        "           "
        " RWWRRWWR  "
        " RWWRRWWRR "
        "RRRRWWWRRRR"
        "RWWRRRRRWRR"
        " RWWRRWWR  "
        " RRRRRRRR  "
        "    WW     "
        "    WW     "
        "   WWWW    "
        "           ";
    draw_img(u, v, img);
}
