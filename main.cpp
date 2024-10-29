//=================================================================
// The main program file.
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================

// project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"
#include <math.h>

#include "mbed.h"

#define CITY_HIT_MARGIN 1
#define CITY_UPPER_BOUND (SIZE_Y-(LANDSCAPE_HEIGHT+MAX_BUILDING_HEIGHT))

// helper function declarations
void playSound(char* wav);


/////////////////////////
// Struct of Player 
/////////////////////////

/**
 * the main game state.
 * includes Player current and previous
 * locations for drawing to work properly.
 */
struct {
    int x,y;                // current locations
    int px, py;             // previous locations
    int p_health;           // previous health
    int health;             // current health
    int max_health;         // max health
    bool has_key;           // flag when obtained the key
    bool game_solved;       // flag when game is complete
    bool talked_to_npc;     // flag when you've talked to npc
    bool ramblin_active;    // flag when ramblin' mode is activated
    bool water_spell;       // flag when water spell equipped
    bool fire_spell;        // flag when fire spell equipped
    bool earth_spell;       // flag when earth spell equipped
    bool teleporting;       // flag when teleport mode is activated
    bool fancy_hat;         // flag when fancy hat has been found
} Player;



/////////////////////////
// Get Action
/////////////////////////

/**
 * given the game inputs, determine what kind of update needs to happen.
 * possible return values are defined below.
 * get actions from user (pushbuttons, and nav_switch)
 * based on push button and navigation switch inputs, determine which action
 * needs to be performed (may be no action).
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define RAMBLIN_MODE 7
#define RUN 8
// #define JUMP 8

int get_action(GameInputs inputs)
{
    // check action and menu button inputs and return the corresponding action value
    if (inputs.b1) return ACTION_BUTTON; // action button is b1
    if (inputs.b2) return MENU_BUTTON; // menu button is b2
    if (inputs.b3) return RAMBLIN_MODE; // activate ramblin' mode with b3

    // check for navigation switch inputs and return the corresponding action value
    if (inputs.ns_left) return GO_LEFT;
    if (inputs.ns_right) return GO_RIGHT;
    if (inputs.ns_up) return GO_UP;
    if (inputs.ns_down) return GO_DOWN;
    if (inputs.ns_center) return RUN;

    // if no button is pressed, return no action value
    return NO_ACTION;
}




/////////////////////////
// Helper Functions
/////////////////////////





/////////////////////////
// Update Game
/////////////////////////

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
int update_game(int action)
{
    // save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;

    // save player previous health before updating
    Player.p_health = Player.health;

    MapItem* item = NULL;

    // variables
    MapItem* north = get_north(Player.x, Player.y);
    MapItem* south = get_south(Player.x, Player.y);
    MapItem* east = get_east(Player.x, Player.y);
    MapItem* west = get_west(Player.x, Player.y);
    MapItem* here = get_here(Player.x, Player.y);


    switch(action)
    {
        ////////////////////////////
        // Player Navigation
        ///////////////////////////

        case GO_UP:
            // check item north
            // if item north is walkable OR ramblin mode is activated
            // player can only walk through a door if they have the key
            if (north->walkable || Player.ramblin_active) {
                // teleport = move 4 tiles at a time
                if (Player.teleporting && get_north(Player.x, Player.y-4)->walkable && get_north(Player.x, Player.y-3)->walkable
                    && get_north(Player.x, Player.y-2)->walkable && get_north(Player.x, Player.y-1)->walkable) {
                    Player.x = Player.x;
                    Player.y -= 4;
                    return FULL_DRAW;
                }
                // then walk up by updating player coordinates
                Player.x = Player.x;
                Player.y -= 1;
            }
            ///////////////////////////////
            // Items that Hurt Player
            ///////////////////////////////
            if (north->type == PEBBLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Ouch!", "You stubbed", "your toe on", "a pebble!!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            if (north->type == HOLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Oh no!", "", "You fell into", "a hole!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            ///////////////////////////////
            // Items that Help Player
            ///////////////////////////////
            if (north->type == POWER_UP) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a power-up!", "Gain 5 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 5;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            if (north->type == MUSHROOM) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a mushroom!", "Gain 10 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 10;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            break; 
        // end go up case

        case GO_LEFT:
            // check item west
            // if item west is walkable OR ramblin mode is activated
            // player can only walk through a door if they have the key
            if (west->walkable || Player.ramblin_active) {
                // teleport = move 4 tiles at a time
                if (Player.teleporting && get_west(Player.x-4, Player.y)->walkable && get_west(Player.x-3, Player.y)->walkable
                    && get_west(Player.x-2, Player.y)->walkable && get_west(Player.x-1, Player.y)->walkable) {
                    Player.x -= 4;
                    Player.y = Player.y;
                    return FULL_DRAW;
                }
                Player.x -= 1;
                Player.y = Player.y;
            }
            ///////////////////////////////
            // Items that Hurt Player
            ///////////////////////////////
            if (west->type == PEBBLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Ouch!", "You stubbed", "your toe on", "a pebble!!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            if (west->type == HOLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Oh no!", "", "You fell into", "a hole!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            ///////////////////////////////
            // Items that Help Player
            ///////////////////////////////
            if (west->type == POWER_UP) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a power-up!", "Gain 5 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 5;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            if (west->type == MUSHROOM) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a mushroom!", "Gain 10 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 10;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            break;
        // end go left case

        case GO_DOWN:
            // check item south
            // if item south is walkable OR ramblin mode is activated
            // player can only walk through a door if they have the key
            if (south->walkable || Player.ramblin_active) {
                // teleport = move 4 tiles at a time
                if (Player.teleporting && get_south(Player.x, Player.y+4)->walkable && get_south(Player.x, Player.y+3)->walkable
                    && get_south(Player.x, Player.y+2)->walkable && get_south(Player.x, Player.y+1)->walkable) {
                    Player.x = Player.x;
                    Player.y += 4;
                    return FULL_DRAW;
                }
                Player.x = Player.x;
                Player.y += 1;
            }
            ///////////////////////////////
            // Items that Hurt Player
            ///////////////////////////////
            if (south->type == PEBBLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Ouch!", "You stubbed", "your toe on", "a pebble!!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            if (south->type == HOLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Oh no!", "", "You fell into", "a hole!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            ///////////////////////////////
            // Items that Help Player
            ///////////////////////////////
            if (south->type == POWER_UP) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a power-up!", "Gain 5 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 5;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            if (south->type == MUSHROOM) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a mushroom!", "Gain 10 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 10;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            break;
        // end go down case

        case GO_RIGHT:
            // check item east
            // if item east is walkable OR ramblin mode is activated
            // player can only walk through a door if they have the key
            if (east->walkable || Player.ramblin_active) {
                // teleport = move 4 tiles at a time
                if (Player.teleporting && get_east(Player.x+4, Player.y)->walkable && get_east(Player.x+3, Player.y)->walkable
                    && get_east(Player.x+2, Player.y)->walkable && get_east(Player.x+1, Player.y)->walkable) {
                    Player.x += 4;
                    Player.y = Player.y;
                    return FULL_DRAW;
                }
                Player.x += 1;
                Player.y = Player.y;
            }
            ///////////////////////////////
            // Items that Hurt Player
            ///////////////////////////////
            if (east->type == PEBBLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Ouch!", "You stubbed", "your toe on", "a pebble!!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            if (east->type == HOLE) {
                // pebbles hurt your toes, so decrease health by 5
                const char *lines[] = {"Oh no!", "", "You fell into", "a hole!", "Lose 10 health."};
                long_speech(lines, 5);
                Player.health -= 10;
                return FULL_DRAW;
            }
            ///////////////////////////////
            // Items that Help Player
            ///////////////////////////////
            if (east->type == POWER_UP) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a power-up!", "Gain 5 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 5;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            if (east->type == MUSHROOM) {
                // power-ups that increase health by 5
                if (Player.health < Player.max_health) {
                    const char *lines[] = {"Yay!", "", "You found", "...", "a mushroom!", "Gain 10 health.", ""};
                    long_speech(lines, 6);
                    Player.health += 10;
                } else {
                    const char *lines[] = {"Already at", "max health."};
                    long_speech(lines, 2);
                }
                return FULL_DRAW;
            }
            break;
        // end go right case

        case RUN: {
            // when center navigation button is pressed, activate teleport mode
            // if already activated, deactive it.
            if (Player.teleporting) {
                Player.teleporting = false;
                const char* speech[] = {"Teleporting mode", "deactivated.", "You now walk at", "normal speed."};
                long_speech(speech, 4);
                return FULL_DRAW;
            } else {
                // activate teleporting mode
                Player.teleporting = true;
                // speech bubble
                const char* speech[] = {"Teleporting mode", "activated.", "You can now move", "4 tiles at once."};
                long_speech(speech, 4);
                return FULL_DRAW;
            }
            break;
        } // end run button case


        //////////////////////////
        // Action Button
        /////////////////////////

        case ACTION_BUTTON:
        {
            /////////////////////////
            // NPC Interactions
            /////////////////////////

            // check if near an NPC
            if (north->type == NPC || south->type == NPC || east->type == NPC || west->type == NPC || here->type == NPC) {
                // if already talked to npc
                if (Player.talked_to_npc && !Player.game_solved) {
                    // speech bubbles
                    const char* speech[] = {"Please don't", "make me say", "the whole speech", "again.", 
                    "Defeat Buzz,", "then you will", "get the keys", "to the Wreck car."};
                    long_speech(speech, 8);
                }
                // if game solved (Buzz defeated)
                else if (Player.talked_to_npc && Player.game_solved) {
                    // give player the key
                    Player.has_key = true;
                    const char* speech[] = {"Congrats!", "You defeated", "the all-powerful", "Wizard Buzz!",
                    "Your reward...", "The keys", "to the", "Ramblin Wreck!"};
                    long_speech(speech, 8);
                }
                // else give instructions on what to do
                else {
                    // player has talked to npc
                    Player.talked_to_npc = true;
                    // show speech bubbles
                    const char* speech[] = {"Hello there!", "How can I help ya", "You wish to drive", "the what...?", "Ramblin Wreck?!",
                    "", "Hmmmm....", "Let's see...", "Well first...", "you need keys!", "In order to get", "the key to that",
                    "ancient car,", "you must prove", "yourself worthy.", "You must be", "brave, smart,", "and determined.",
                    "You must defeat", "Wizard Buzz!", "Only then, will", "you receive the", "famous keys.", "Here's a tip:", 
                    "fire is Buzz's", "greatest enemy.", "Oh and watch out", "for items that", "might hurt you", "and keep an eye",
                    "out for power-ups.", "Good luck,", "brave stranger."};
                    long_speech(speech, 33);
                }
                // return FULL_DRAW to redraw the game
                return FULL_DRAW;
            }

            ///////////////////////////
            // Opening Doors
            //////////////////////////

            // check if near a door
            if (north->type == DOOR || south->type == DOOR || east->type == DOOR || west->type == DOOR || here->type == DOOR) {
                // if player has the key, game is won. return GAME_OVER
                if (Player.has_key) {
                    return GAME_OVER;
                } else {
                    // show speech bubbles that the player needs to get the key
                    const char* speech[] = {"Ramblin Wreck", "car is on the", "other side of", "this locked door", 
                    "Find the key", "and try again."};
                    long_speech(speech, 4);
                    // return FULL_DRAW to redraw the scene
                    return FULL_DRAW;
                }
            }

            /////////////////////////////
            // Entering Buzz's Cave
            ////////////////////////////

            // check if on Buzz's cave
            if (north->type == CAVE || south->type == CAVE || east->type == CAVE || west->type == CAVE || here->type == CAVE) {
                // if the player has talked to the npc, enter the cave.
                if (Player.talked_to_npc) {
                    // start speech bubble to fight buzz
                    const char* speech[] = {"You are about to", "enter the portal", "to Buzz's cave..."};
                    long_speech(speech, 3);
                    // set map to small map
                    Map *small = set_active_map(1);
                    // set player coordinates to small map
                    Player.x = Player.y = map_width()/4;
                } else {
                    // speech bubbles to talk to npc
                    const char* speech[] = {"Hmmm...", "what an ", "interesting", "cave...", "You might",
                    "want to try", "talking to", "someone more", "knowledgeable.."};
                    long_speech(speech, 9);
                }
                // return FULL_DRAW to redraw the scene
                return FULL_DRAW;
            }

            ///////////////////////////
            // Entering Secret Map
            //////////////////////////
            if (north->type == SECRET_DOOR || south->type == SECRET_DOOR || east->type == SECRET_DOOR 
                || west->type == SECRET_DOOR || here->type == SECRET_DOOR)
            {
                // start speech bubble to fight buzz
                const char* speech[] = {"You have found", "the secret door", "Now entering the", "secret map."};
                long_speech(speech, 4);
                // set map to secret map
                Map *secret = set_active_map(2);
                // set player coordinates to small map
                Player.x = Player.y = 5;
                return FULL_DRAW;
            }

            ///////////////////////////
            // Using Stairs
            //////////////////////////

            // check if on a stairs
            if (north->type == STAIRS || south->type == STAIRS || east->type == STAIRS || west->type == STAIRS || here->type == STAIRS) {
                if (get_active_map_index() == 1) {
                    // move player back to cave entrance
                    Player.x = 5;
                    Player.y = 20;
                    // speech bubbles
                    const char* speech[] = {"You are about to", "take the portal", "out of the cave"};
                    long_speech(speech, 3);
                    // set map back to main big map
                    set_active_map(0);
                    return FULL_DRAW;
                }
                else if (get_active_map_index() == 2) {
                    // move player back to cave entrance
                    Player.x = 48;
                    Player.y = 48;
                    // speech bubbles
                    const char* speech[] = {"You are about to", "take the door", "back to the", "main map."};
                    long_speech(speech, 4);
                    // set map back to main big map
                    set_active_map(0);
                    return FULL_DRAW;
                }
                // return FULL_DRAW to redraw the scene
                return FULL_DRAW;
            }

            /////////////////////////////
            // Gift Box
            ////////////////////////////
            if (north->type == GIFT_BOX || south->type == GIFT_BOX || east->type == GIFT_BOX || west->type == GIFT_BOX 
                || here->type == GIFT_BOX) {
                    const char* lines[] = {"You found a", "gift box!", "Opening box", "....", "A brand new", 
                    "hat has been", "equipped!", ""};
                    long_speech(lines, 8);
                    Player.fancy_hat = true;
                    // can only find gift box once, so erase it
                    // draw_nothing(0, 25);
                    return FULL_DRAW;
            }

            //////////////////////////////
            // Equipping Spells
            /////////////////////////////

            // check if near a WATER spell
            if (north->type == WATER || south->type == WATER || east->type == WATER || west->type == WATER || here->type == WATER) {
                // can only equip a spell once
                if (Player.water_spell) {
                    speech("WATER spell", "already equipped");
                } else {
                    // water spell equipped
                    Player.water_spell = true;
                    // speech bubble
                    speech("WATER spell", "equipped.");
                }
                // return FULL_DRAW to redraw the scene
                return FULL_DRAW;
            }
            // check if near a FIRE spell
            if (north->type == FIRE || south->type == FIRE || east->type == FIRE || west->type == FIRE || here->type == FIRE) {
                // can only equip a spell once
                if (Player.fire_spell) {
                    speech("FIRE spell", "already equipped");
                } else {
                    // water spell equipped
                    Player.fire_spell = true;
                    // speech bubble
                    speech("FIRE spell", "equipped.");
                }
                // return FULL_DRAW to redraw the scene
                return FULL_DRAW;
            }
            // check if near an EARTH spell
            if (north->type == EARTH || south->type == EARTH || east->type == EARTH || west->type == EARTH || here->type == EARTH) {
                // can only equip a spell once
                if (Player.earth_spell) {
                    speech("EARTH spell", "already equipped");
                } else {
                    // water spell equipped
                    Player.earth_spell = true;
                    // speech bubble
                    speech("EARTH spell", "equipped.");
                }
                // return FULL_DRAW to redraw the scene
                return FULL_DRAW;
            }

            /////////////////////////////////
            // Winning the Game
            ////////////////////////////////

            // check if near Buzz:
            if (north->type == BUZZ || south->type == BUZZ || east->type == BUZZ || west->type == BUZZ || here->type == BUZZ) {
                // check which spell is equipped
                // if water -> Buzz just got very angry. not effective.
                if (Player.water_spell) {
                    // speech bubbles
                    const char* speech[] = {"Press button to", "cast water spell.", "Water spell cast", "...", 
                    "That spell made", "Buzz very angry.", "Damage: 25", "Try another one."};
                    long_speech(speech, 8);
                    // hurt player
                    Player.health -= 25;
                    // unequip spell
                    Player.water_spell = false;
                    // return FULL_DRAW to redraw the scene
                    return FULL_DRAW;
                }
                // if earth -> spell not very effective
                if (Player.earth_spell) {
                    // speech bubbles
                    const char* speech[] = {"Press button to", "cast earth spell.", "Earth spell cast", "...", 
                    "Spell was not", "effective at all.", "Damage: 15", "Try another one"};
                    long_speech(speech, 7);
                    // hurt player
                    Player.health -= 15;
                    // unequip spell
                    Player.earth_spell = false;
                    // return FULL_DRAW to redraw the scene
                    return FULL_DRAW;
                }
                // if fire -> Buzz defeated.
                if (Player.fire_spell) {
                    // speech bubbles
                    const char* speech[] = {"Press button to", "cast fire spell.", "Fire spell cast", "...", 
                    "Success!", "", "Wizard Buzz", "defeated.", "Return to the one", "who gave you", "your quest to", 
                    "receive the key."};
                    long_speech(speech, 12);
                    // unequip spell
                    Player.fire_spell = false;
                    // flag that the game is solved
                    Player.game_solved = true;
                    // draw Slain Buzz
                    add_slain_buzz(map_width()/2, map_width()/2);
                    // return FULL_DRAW to redraw the scene
                    return FULL_DRAW;
                }
                else {
                    const char* speech[] = {"No spell", "equipped.", "Damage: 5", ""};
                    long_speech(speech, 4);
                    Player.health -= 5;
                    return FULL_DRAW;
                }
            }

            else {
                speech("Showing game", "configuration...");
                draw_config();
                // wait for action button to continue
                while (1) {
                    if (!button1) {
                        break;
                    }
                }
                uLCD.cls();
                return FULL_DRAW;
            }
            break;
        }
        // end action button case

        ///////////////////////////
        // Menu Button
        ///////////////////////////

        case MENU_BUTTON:
        {
            // show inventory of spells
            char* num_water = (char*)"none";
            char* num_earth = (char*)"none";
            char* num_fire = (char*)"none";
            char* hat = (char*)"none";
            if (Player.water_spell) {
                num_water = (char*)"equipped";
            }
            if (Player.earth_spell) {
                num_earth = (char*)"equipped";
            }
            if (Player.fire_spell) {
                num_fire = (char*)"equipped";
            }
            if (Player.fancy_hat) {
                hat = (char*)"equipped";
            }
            const char* speech[] = {"Inventory", "of Spells...", "Water Spell:", num_water, 
            "Earth Spell:", num_earth, "Fire Spell:", num_fire, "Fancy Hat:", hat, ""};
            long_speech(speech, 11);
            // return FULL_DRAW to redraw the scene
            return FULL_DRAW;
            break;
        }
        // end menu button case

        ////////////////////////////
        // Ramblin' Mode Button
        ////////////////////////////

        case RAMBLIN_MODE:
        {
            // if already activated, deactive it.
            if (Player.ramblin_active) {
                Player.ramblin_active = false;
                const char* speech[] = {"Ramblin' mode", "deactivated.", "You cannot walk", "through walls"};
                long_speech(speech, 4);
                return FULL_DRAW;
            } else {
                // activate ramblin mode
                Player.ramblin_active = true;
                // speech bubble
                const char* speech[] = {"Ramblin' mode", "activated.", "You can now walk", "through walls"};
                long_speech(speech, 4);
                return FULL_DRAW;
            }
            break;
        }
        // end ramblin mode button case
    }
    // if none of these cases, return NO_RESULT
    return NO_RESULT;
}




/////////////////////////
// Draw Game
/////////////////////////

/**
 * entry point for frame drawing.
 * called once per iteration of the game loop.
 * this draws all tiles on the screen, followed by the status bars.
 * unless init is nonzero, this function will optimize drawing by only
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // draw game border first
    if(init) draw_border();
    // iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // iterate over one column of tiles
        {
            // given (i,j)
            // compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;

            // compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;

            // compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;

            // figure out what to draw
            DrawFunc draw = NULL;
//            if (init && i == 0 && j == 0) // only draw the player on init
            if ( i == 0 && j == 0) // always draw the player
            {
                draw_player(u, v, Player.has_key, Player.fancy_hat);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
                else if (curr_item && curr_item->type == CLEAR)
                {
                    // this is a special case for erasing things like doors.
                    draw = curr_item->draw; // i.e. draw_nothing
                } 
            }
            else if (init) // if doing a full draw, but out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }
    // draw status bars
    if(init) {
        uLCD.filled_rectangle(0, 0, 127, 17, 0xf6f6f6);
        draw_upper_status(Player.x, Player.y, Player.has_key);
        uLCD.filled_rectangle(0, 112, 127, 117, 0xf6f6f6);
        draw_lower_status(Player.health);
    }
    if (Player.px != Player.x || Player.py != Player.y) {
        uLCD.filled_rectangle(0, 0, 127, 17, 0xf6f6f6);
        draw_upper_status(Player.x, Player.y, Player.has_key);
        uLCD.filled_rectangle(0, 112, 127, 117, 0xf6f6f6);
        draw_lower_status(Player.health);
    }
}







/////////////////////////
// Map Intialization
/////////////////////////

// important locations for all maps
// location of the center of the cave
int cb_loc[2] = {5,20};
// location of the entrance to secret map


/**
 * initialize the main world map.
 * add walls around the edges, interior chambers,
 * and plants in the background so you can see motion.
 */
void init_main_map()
{
    // initialize and set current map to the first map
    Map* map = set_active_map(0);

    /////////////////////////
    // Initial Environmnet
    /////////////////////////

    // Add random plants
    pc.printf("Adding Plants!\r\n");
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
        add_bush((i + 5)/ map_height(), (i+5) % map_height());
    }
    for (int i = map_width() + 3; i < map_area(); i += 63)
    {
        add_pebble(i / (map_width()/2), i % map_width() + 5);
        add_power_up(i % map_height() - 2, i / (map_height()/3));
    }

    // add wall borders 
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    
    // add extra chamber borders 
    pc.printf("Add extra chamber\r\n");
    add_wall(30, 0, VERTICAL, 10);
    add_wall(30, 10, HORIZONTAL, 10);
    add_wall(39, 0, VERTICAL, 10);
    add_door(33, 10, HORIZONTAL, 4);

    // add extra cave to Buzz's evil lair
    pc.printf("Add cave\r\n");
    add_cave(cb_loc[0],cb_loc[1],1,1,5,5);      // cave is set as a 4x4 block to be bigger
    add_cave(cb_loc[0]+1,cb_loc[1],2,1,5,5);
    add_cave(cb_loc[0],cb_loc[1]+1,3,1,5,5);
    add_cave(cb_loc[0]+1,cb_loc[1]+1,4,1,5,5);

    pc.printf("Initial environment completed\r\n");

    ///////////////////////////////////
    // Characters and Items for Map
    ///////////////////////////////////

    // add NPC
    add_npc(20, 5);

    // add any extra characters/items here for your project
    add_wreck(35, 9);
    // add_gift_box(48, 48);
    add_hole(10, 10);
    add_hole(20, 20);
    add_hole(30, 30);
    add_hole(40, 40);

    // add secret entrance to another map
    add_secret_entrance(48, 48, 1, 4, 4);

    // print out map
    print_map();
}






/**
 * initialize the small map.
 * add walls around the edges, Wizard Buzz in the center,
 * spells for the player to use to defeat Buzz, 
 * and stairs back up to the main map.
**/
void init_small_map()
{
    // set current map to the second map
    Map* small = set_active_map(1);

    // add wall borders to small map
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,         HORIZONTAL, 16);
    add_wall(0,              15,        HORIZONTAL, 16);
    add_wall(0,              0,         VERTICAL,   16);
    add_wall(15,             0,         VERTICAL,   16);

    // add three spells at different locations
    add_water(5, 5);
    add_fire(13, 1);
    add_earth(8, 12);

    // add Wizard Buzz at the center of the map
    add_buzz(map_width()/2, map_height()/2);

    // add stairs back to main (map 0)
    add_stairs(4, 6, 0, cb_loc[0], cb_loc[1]);

    // additional items
    add_pebble(9,5);
    add_power_up(13, 12);

    // print out map
    print_map();
}



/**
 * initialize the secret map.
 * add walls around the edges, gift box in the center,
 * power-ups for the player to gain health,
 * and stairs back up to the main map.
**/
void init_secret_map()
{
    // set current map to the second map
    Map* secret = set_active_map(2);

    // add wall borders to small map
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,          HORIZONTAL, 12);
    add_wall(0,              11,         HORIZONTAL, 12);
    add_wall(0,              0,          VERTICAL,   12);
    add_wall(11,             0,          VERTICAL,   12);

    // add stairs back to main (map 0)
    add_secret_stairs(4, 6, 0, 48, 48);

    // additional items
    add_power_up(9, 9);
    add_power_up(7, 7);
    add_mushroom(10, 10);
    
    // add gift box to center of map
    add_gift_box(map_width()/2+1, map_height()/2+1);

    // print out map
    print_map();
}


/**
 * program entry point!
 * this function orchestrates all the parts of the game.
 * game loop:
 *   - read hardware inputs
 *   - update game based on inputs
 *   - draw the game
 *   - frame delay
 */
int main()
{
    // initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");

    // draw start up screen
    draw_start_up();
    // wait for action button to continue
    while (1) {
        if (!button1) {
            break;
        }
    }
    uLCD.cls();

    // draw game configuration info screen
    draw_config();
    // wait for action button to continue
    while (1) {
        if (!button1) {
            break;
        }
    }
    uLCD.cls();

    // draw loading screen
    uLCD.cls();
    uLCD.color(WHITE);
    uLCD.textbackground_color(BLACK);
    uLCD.text_height(2);
    uLCD.text_width(2);
    uLCD.locate(1, 1);
    uLCD.printf("Loading\n Game...\n");
    uLCD.text_width(3);
    uLCD.text_height(3);
    uLCD.color(RED);
    for (int i=10; i>=0; --i) {
        uLCD.locate(1,3);
        uLCD.printf("%2D",i);
        wait(.5);
    }
    uLCD.cls();

    // initialize the maps
    maps_init();
    init_main_map();
    init_small_map();
    init_secret_map();
    
    // initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;
    Player.has_key = false;
    Player.game_solved = false;
    Player.talked_to_npc = false;
    Player.health = Player.max_health = 50;

    // initial drawing
    draw_game(true);

    ////////////////////////
    // Main Game Loop
    ///////////////////////

    while(1)
    {
        // timer to measure game update speed
        Timer t; t.start();

        // game update
        // read inputs
        GameInputs in = read_inputs();

        // determine action (get_action)
        int action = get_action(in);

        // update game
        // set this variable "result" for the resulting state after update game
        int result = update_game(action);
        // check for game over based on update game result
        if (Player.health <= 0) {
            const char* array[] = {"Oh no!", "", "You have no more", "health points.", ""};
            long_speech(array, 5);
            // show game over screen
            uLCD.cls();
            uLCD.background_color(BLACK);
            uLCD.text_width(4);
            uLCD.text_height(3);
            uLCD.color(0xffffe0);
            uLCD.textbackground_color(BLACK);
            uLCD.printf("\nYOU\n\nLOSE");
            wait(5);
            uLCD.cls();
            uLCD.background_color(BLACK);
            uLCD.text_width(2);
            uLCD.text_height(2);
            uLCD.color(0xffffe0);
            uLCD.textbackground_color(BLACK);
            uLCD.printf("\nPush\nReset to\nRetry.");
            break;
        }
        if (result == GAME_OVER) {
            const char* array[] = {"Door unlocked!", "", "The 2nd key", "starts the car.", "Push action again", "to end the game!"};
            long_speech(array, 6);
            draw_game(true);
            // wait for action button to continue
            while (1) {
                if (!button1) {
                    break;
                }
            }
            // show game over screen
            uLCD.cls();
            uLCD.background_color(BLACK);
            uLCD.text_width(4);
            uLCD.text_height(3);
            uLCD.color(0xffffe0);
            uLCD.textbackground_color(BLACK);
            uLCD.printf("\nYOU\n\nWIN!");
            // wait for action button to continue
            while (1) {
                if (!button1) {
                    break;
                }
            }
            uLCD.cls();
            uLCD.background_color(BLACK);
            uLCD.text_width(2);
            uLCD.text_height(2);
            uLCD.color(0xffffe0);
            uLCD.textbackground_color(BLACK);
            uLCD.locate(2, 1);
            uLCD.printf("Push\n\nReset to \n Replay");
            break;
        }

        // draw screen to uLCD
        bool full_draw = false;
        if (result == FULL_DRAW) full_draw = true;
        draw_game(full_draw);
        // frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}




/////////////////////////////
// Advanced Features
/////////////////////////////

// plays a wavfile
void playSound(char* wav)
{
    // open wav file
    FILE *wave_file;
    wave_file = fopen(wav,"r");

    if(wave_file != NULL) 
    {
        printf("File opened successfully\n");

        // play wav file
        printf("Sound playing...\n");
        waver.play(wave_file);

        // close wav file
        printf("Sound stopped...\n");
        fclose(wave_file);
        return;
    }
    printf("Could not open file for reading - %s\n", wav);
    return;
}
