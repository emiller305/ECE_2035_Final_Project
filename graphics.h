// ============================================
// The graphics header file
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================

#ifndef GRAPHICS_H
#define GRAPHICS_H

/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);

/**
 * Draws the player. This depends on the player state, so it is not a DrawFunc.
 */
void draw_player(int u, int v, int key, bool gift);

/**
 * Draw new door once player has one the game.
*/
void draw_new_door(int u, int v);

/**
 * Draw the upper status bar.
 * Shows Player current status
 */
void draw_upper_status(int x, int y, bool k);

/**
 * Draw the lower status bar.
 */ 
void draw_lower_status(int h);

/**
 * Draw the border for the map.
 */
void draw_border();

/**
 * Draw the screen for game configuration.
*/
void draw_config();

/**
 * Draw start up page.
*/
void draw_start_up();

/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
void draw_wall(int u, int v);
void draw_plant(int u, int v);
void draw_mud(int u, int v);
void draw_door(int u, int v);
void draw_npc(int u, int v);
void draw_stairs(int u, int v);
void draw_cave1(int u, int v);
void draw_cave2(int u, int v);
void draw_cave3(int u, int v);
void draw_cave4(int u, int v);
void draw_water(int u , int v);
void draw_fire(int u , int v);
void draw_earth(int u , int v);
void draw_buzz(int u, int v);
void draw_wreck(int u, int v);
void draw_slain_buzz(int u, int v);
void draw_pebble(int u, int v);
void draw_power_up(int u, int v);
void draw_gift_box(int u, int v);
void draw_bush(int u, int v);
void draw_hole(int u, int v);
void draw_secret_entrance(int u, int v);
void draw_secret_stairs(int u, int v);
void draw_mushroom(int u, int v);

#endif // GRAPHICS_H