//=================================================================
// The map class file.
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================

#include "map.h"
#include "hash_table.h"
#include "globals.h"
#include "graphics.h"
#include "hash_table.h"

/**
 * the Map structure.
 * this holds a HashTable for all the MapItems,
 * along with values for the width and height of the Map.
 */
struct Map {
    HashTable* items; // hashtables for all items of the map
    int w, h;         // map dimensions
    int index;        // index of map (i.e., first map or second map)
};

//////////////////////////////
// Important Definitions
/////////////////////////////

#define MHF_NBUCKETS 97     //  hashing value
#define NUM_MAPS 3          //  number of total maps. can add more
static Map maps[NUM_MAPS];  //  array of maps
static int active_map;      //  current active map on screen


// to erase a MapItem, we can simply replace it
// with a clear type item called clear sentinel
static const MapItem CLEAR_SENTINEL = { 
    .type = CLEAR,
    .draw = draw_nothing
};


/**
 * the first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * this function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    // teturn the 1-D XY key
    return X + Y * map_width();
}

/**
 * this is the hash function actually passed into createHashTable.
 * it takes an unsigned key (the output of XY_KEY) 
 * and turns it into a hash value.
 */
unsigned map_hash(unsigned key)
{
    // use MHF_NBUCKETS as the value to hash for your hash function
    // return the hashed key
    return key % MHF_NBUCKETS;
}

/**
 * initializes the map, using a hash_table, setting the width and height.
 */
void maps_init()
{
    // loop through all possible maps, where for each map's items, create a hashtable
    for (int i = 0; i < NUM_MAPS; i++) {
        maps[i].items = createHashTable(map_hash, MHF_NBUCKETS);
        // set width & height for any maps
        // main map is 50x50
        if (i == 0) {
            maps[i].index = 0;
            maps[i].h = 50;
            maps[i].w = 50;
        }
        // smaller map is 16x16
        else if (i == 1) {
            maps[i].index = 1;
            maps[i].h = 16;
            maps[i].w = 16;
        }
        // secret map is 12x12
        else {
            maps[i].index = 2;
            maps[i].h = 12;
            maps[i].w = 12;
        }
        // set the first map to be active
        if (i == 0) active_map = i;
    }
}


Map* get_active_map()
{
    // return a pointer to the current map based on which map is active (active_map)
    return &maps[active_map];
}

int get_active_map_index()
{
    // return the index to the current map based on which map is active (active_map)
    return maps[active_map].index;
}

Map* set_active_map(int m)
{
    // set the global variable for active map to the map index passed in
    active_map = m;
    // update the index for the active map to the index passed in
    maps[active_map].index = m;
    // return a pointer to the current map based on which map is active (active_map)
    return &maps[active_map];
}

/**
 *  prints out the map for debugging on the terminal
 */

void print_map()
{
    // NOTE: As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'D', 'P', 'A', 'K', 'C', 'N',' ','S'};
    Map* map = get_active_map();
    for(int j = 0; j < map->h; j++)
    {
        for (int i = 0; i < map->w; i++)
        {
            MapItem* item = (MapItem*)getItem(map->items, XY_KEY(i, j));
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}


/**
 * returns width of active map
 */
int map_width()
{
    // get the current active map
    Map *active = get_active_map();
    // check the struct of active map item
    if (active) {
        // return the width of active map
        return active->w;
    }
    return NULL;
}

/**
 * returns height of active map
 */
int map_height()
{
    // get the current active map
    Map *active = get_active_map();
    // checl the struct of active map item
    if (active) {
        // return the height of active map
        return active->h;
    }
    return NULL;
}

/**
 * returns the area of the active map
 */
int map_area()
{
    // compute and return the area of the active map
    return map_height() * map_width();
}

/**
 * returns the MapItem immediately above the given location.
 */
MapItem* get_north(int x, int y)
{
    // get map item
    HashTable *ht = maps[get_active_map_index()].items;
    MapItem *item = (MapItem*)getItem(ht, XY_KEY(x, y-1));
    // if the item exist and is a clear type, remove item
    if (item != NULL && item->type == CLEAR) {
        removeItem(ht, XY_KEY(x, y-1));
    }
    // return the item;
    return item;
}

/**
 * returns the MapItem immediately below the given location.
 */
MapItem* get_south(int x, int y)
{
    // get map item
    HashTable *ht = maps[get_active_map_index()].items;
    MapItem *item = (MapItem*)getItem(ht, XY_KEY(x, y+1));
    // if the item exist and is a clear type, remove item
    if (item != NULL && item->type == CLEAR) {
        removeItem(ht, XY_KEY(x, y+1));
    }
    // return the item
    return item;
}

/**
 * returns the MapItem immediately right the given location.
 */
MapItem* get_east(int x, int y)
{
    // get map item
    HashTable *ht = maps[get_active_map_index()].items;
    MapItem *item = (MapItem*)getItem(ht, XY_KEY(x+1, y));
    // if the item exist and is a clear type, remove item
    if (item != NULL && item->type == CLEAR) {
        removeItem(ht, XY_KEY(x+1, y));
    }
    // return the item
    return item;
}

/**
 * returns the MapItem immediately left the given location.
 */
MapItem* get_west(int x, int y)
{
    // get map item
    HashTable *ht = maps[get_active_map_index()].items;
    MapItem *item = (MapItem*)getItem(ht, XY_KEY(x-1, y));
    // if the item exist and is a clear type, remove item
    if (item != NULL && item->type == CLEAR) {
        removeItem(ht, XY_KEY(x-1, y));
        // return item;
    }
    // return the item
    return item;
}

/**
 * returns the MapItem at current coordinate location
 */
 MapItem* get_here(int x, int y)
 {
    // get map item 
    HashTable *ht = maps[get_active_map_index()].items;
    MapItem *item = (MapItem*)getItem(ht, XY_KEY(x, y));
    // if the item exist and is a clear type, remove item
    if (item != NULL && item->type == CLEAR) {
        removeItem(ht, XY_KEY(x, y));
    }
    // return the item
    return item;
 }
 

/**
 * erases item on a location by replacing it with a clear sentinel
 */
void map_erase(int x, int y)
{
    MapItem* item = (MapItem*)insertItem(get_active_map()->items, XY_KEY(x, y), (void*)&CLEAR_SENTINEL);
    if(item) free(item);
}


/////////////////////////////////////////
// Adding Specific Items to the Map
////////////////////////////////////////

void add_plant(int x, int y)
{
    MapItem* p = (MapItem*)malloc(sizeof(MapItem));
    p->type = PLANT;
    p->draw = draw_plant;
    p->walkable = true;
    p->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), p);
    if (val) free(val); // if something is already there, free it
}

void add_npc(int x, int y)
{
    MapItem* npc1 = (MapItem*)malloc(sizeof(MapItem));
    npc1->type = NPC;
    npc1->draw = draw_npc;
    npc1->walkable = false;
    npc1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), npc1);
    if (val) free(val);
}

void add_water(int x, int y)
{
    MapItem* w = (MapItem*)malloc(sizeof(MapItem));
    w->type = WATER;
    w->draw = draw_water;
    w->walkable = true;
    w->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w);
    if (val) free(val);
}

void add_fire(int x, int y)
{
    MapItem* f = (MapItem*)malloc(sizeof(MapItem));
    f->type = FIRE;
    f->draw = draw_fire;
    f->walkable = true;
    f->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), f);
    if (val) free(val);
}

void add_earth(int x, int y)
{
    MapItem* e = (MapItem*)malloc(sizeof(MapItem));
    e->type = EARTH;
    e->draw = draw_earth;
    e->walkable = true;
    e->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), e);
    if (val) free(val);
}


void add_buzz(int x, int y)
{
    MapItem* b = (MapItem*)malloc(sizeof(MapItem));
    b->type = BUZZ;
    b->draw = draw_buzz;
    b->walkable = false;
    b->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), b);
    if (val) free(val);
}

void add_slain_buzz(int x, int y)
{
    // this function is to ovewrite Buzz when he is defeated
    MapItem* sb = (MapItem*)malloc(sizeof(MapItem));
    sb->type = SLAIN_BUZZ;
    sb->draw = draw_slain_buzz;
    sb->walkable = false;
    sb->data = NULL; 
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), sb);
    if (val) free(val);
}

void add_wreck(int x, int y) {
    MapItem* rw = (MapItem*)malloc(sizeof(MapItem));
    rw->type = RAMBLIN_WRECK;
    rw->draw = draw_wreck;
    rw->walkable = false;
    rw->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), rw);
    if (val) free(val);
}

void add_pebble(int x, int y)
{
    MapItem* p = (MapItem*) malloc(sizeof(MapItem));
    p->type = PEBBLE;
    p->draw = draw_pebble;
    p->walkable = true;
    p->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), p);
    if (val) free(val); // if something is already there, free it
}

void add_power_up(int x, int y)
{
    MapItem* pu = (MapItem*) malloc(sizeof(MapItem));
    pu->type = POWER_UP;
    pu->draw = draw_power_up;
    pu->walkable = true;
    pu->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), pu);
    if (val) free(val); // if something is already there, free it
}

void add_gift_box(int x, int y)
{
    MapItem* gb = (MapItem*) malloc(sizeof(MapItem));
    gb->type = GIFT_BOX;
    gb->draw = draw_gift_box;
    gb->walkable = false;
    gb->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), gb);
    if (val) free(val); // if something is already there, free it

}

void add_bush(int x, int y)
{
    MapItem* b = (MapItem*) malloc(sizeof(MapItem));
    b->type = BUSH;
    b->draw = draw_bush;
    b->walkable = true;
    b->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), b);
    if (val) free(val); // if something is already there, free it
}

void add_hole(int x, int y)
{
    MapItem* h = (MapItem*) malloc(sizeof(MapItem));
    h->type = HOLE;
    h->draw = draw_hole;
    h->walkable = true;
    h->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), h);
    if (val) free(val); // if something is already there, free it
}

///////////////////////////////////////
// Adding Environment to the Map
///////////////////////////////////////


void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // if something is already there, free it
    }
}

void add_door(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = DOOR;
        w1->draw = draw_door;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // if something is already there, free it
    }
}


void add_stairs(int x, int y, int tm, int tx, int ty)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = STAIRS;
    w1->draw = draw_stairs;
    w1->walkable = true;
    StairsData* data = (StairsData*) malloc(sizeof(StairsData));
    data->tm = tm;
    data->tx = tx;
    data->ty = ty;
    w1->data = data;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // if something is already there, free it
}


void add_cave(int x, int y, int n, int tm, int tx, int ty)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = CAVE;
    if (n==1){
        w1->draw = draw_cave1;
    }
    if (n==2){
        w1->draw = draw_cave2;
    }
    if (n==3){
        w1->draw = draw_cave3;
    }
    if (n==4){
        w1->draw = draw_cave4;
    }
    w1->walkable = true;
    StairsData* data = (StairsData*) malloc(sizeof(StairsData));
    data->tm = tm;
    data->tx = tx;
    data->ty = ty;
    w1->data = data;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // if something is already there, free it
}


void add_mud(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = MUD;
        w1->draw = draw_mud;
        w1->walkable = true;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // if something is already there, free it
    }
}

void add_secret_entrance(int x, int y, int tm, int tx, int ty)
{
    MapItem* e = (MapItem*) malloc(sizeof(MapItem));
    e->type = SECRET_DOOR;
    e->draw = draw_secret_entrance;
    e->walkable = true;
    StairsData* data = (StairsData*) malloc(sizeof(StairsData));
    data->tm = tm;
    data->tx = tx;
    data->ty = ty;
    e->data = data;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), e);
    if (val) free(val); // if something is already there, free it
}

void add_secret_stairs(int x, int y, int tm, int tx, int ty)
{
    MapItem* s = (MapItem*) malloc(sizeof(MapItem));
    s->type = STAIRS;
    s->draw = draw_secret_stairs;
    s->walkable = true;
    StairsData* data = (StairsData*) malloc(sizeof(StairsData));
    data->tm = tm;
    data->tx = tx;
    data->ty = ty;
    s->data = data;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), s);
    if (val) free(val); // if something is already there, free it
}

void add_mushroom(int x, int y)
{
    MapItem* m = (MapItem*) malloc(sizeof(MapItem));
    m->type = MUSHROOM;
    m->draw = draw_mushroom;
    m->walkable = true;
    m->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x,y), m);
    if (val) free(val); // if something is already there, free it
}
