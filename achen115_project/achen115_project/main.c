/*
 * achen115_project.c
 *
 * Created: 5/8/2019 3:24:34 PM
 * Author : Alex
 */ 

#include <avr/io.h>
#include "nokia5110.c"
#include "timer.h"

#define FOR_TETRA for(unsigned short i = 0; i < 4; i++)
#define WIDTH 12
#define HEIGHT 21
#define DDR_BUTTONS DDRD
#define PIN_BUTTONS PIND

void store(unsigned char address, unsigned char data)
{
	while(EECR & 4);//Wait for write
	EEAR = address;	//Set address
	EEDR = data;	//Set data
	EECR &= 0x0F;	//Write mode
	EECR |= 4;		//Master write enable
	EECR |= 2;		//Write enable
}
unsigned char load(unsigned int address) {
	while(EECR & 4);//Wait for write
	EEAR = address;	//Set address
	EECR |= 1;		//Read enable
	return EEDR;	//Done
}
typedef struct Point {
	signed short x, y;
} Point;
char inWidth(Point *p) {
	return p->x > -1 && p->x < WIDTH;
}
Point add(Point *p1, Point *p2) {
	Point result;
	result.x = p1->x + p2->x;
	result.y = p1->y + p2->y;
	return result;
}
// Rotates a point 90 degrees CCW around the origin
// We store the Tetromino tiles as relative positions
// So we can use a neat math trick to do this
// tx = x
// x = -y
// y = tx
// ( 1, 0)  ( 0, 1)  (-1, 0)  ( 0,-1)
//   ...      .+.      ...      ...
//   ..+      ...      +..      ...
//   ...      ...      ...      .+.
void pivot(Point* p) {
	signed short x = p->x;
	p->x = -p->y;
	p->y = x;
}

//A tetromino, a set of four adjacent points around a position
typedef struct Tetra {
	Point pos;			//Absolute position
	Point tiles[4];		//Relative to pos
} Tetra;
//	nn   n    ne   e    se   s    sw   w    nw   c
//  .+.  ...  ...  ...  ...  ...  ...  ...  ...  ...
//  ...  .+.  ..+  ...  ...  ...  ...  ...  +..  ...
//  ...  ...  ...  ..+  ...  ...  ...  +..  ...  .+.
//  ...  ...  ...  ...  ..+  .+.  +..  ...  ...  ...
//Tetromino tile templates
#define nn (Point) { .x = 0, .y = 2 }	//North North
#define n (Point) { .x = 0, .y = 1 }	//North
#define ne (Point) { .x = 1, .y = 1 }	//Northeast
#define e (Point) { .x = 1, .y = 0 }	//East
#define se (Point) { .x = 1, .y = -1 }	//Southeast
#define s (Point) { .x = 0, .y = -1 }	//South
#define sw (Point) { .x = -1, .y = -1 }	//Southwest
#define w (Point) { .x = -1, .y = 0 }	//West
#define nw (Point) { .x = -1, .y = 1 }	//Northwest
#define c (Point) { .x = 0, .y = 0 }	//Center
// Tetromino templates
//  ...  ...  .+.  ...
//  .+.  .++  .+.  .++
//  .+.  ++.  .+.  .++
//  .++  ...  .+.  ...
// Points are copied on assignment
Tetra L(Point pos) { return (Tetra) {.pos = pos, .tiles = {n, c, s, se}}; }
Tetra Z(Point pos) { return (Tetra) {.pos = pos, .tiles = {w, c, e, ne}}; }
Tetra I(Point pos) { return (Tetra) {.pos = pos, .tiles = {s, c, n, nn}}; }
Tetra O(Point pos) { return (Tetra) {.pos = pos, .tiles = {s, c, e, se}}; }
//Creates a random tetromino
Tetra CreateTetra() {
	Point pos = {.x = WIDTH/2, .y = HEIGHT + 2};
	switch(rand()%4) {
	case 0: return L(pos);
	case 1: return Z(pos);
	case 2: return I(pos);
	default:
	case 3: return O(pos);
	}
}
//Returns a copy of an absolute point on the tetromino
Point getTile(Tetra *t, unsigned short index) {
	return add(&t->pos, &t->tiles[index]);
}
//Decrements the tetromino position without bounds checking
void down(Tetra *t) {
	t->pos.y--;
}
//Movs the tetromino position right without bounds checking
void right(Tetra *t) {
	t->pos.x++;
}
//Movs the tetromino position left without bounds checking
void left(Tetra *t) {
	t->pos.x--;
}
//Flips the tetromino tiles without bounds checking
void mirror(Tetra* t) {
	for(unsigned short i = 0; i < 4; i++) {
		t->tiles->x = -t->tiles->x;
	}
}
//Rotates the tetromino tiles around the center without bounds checking
void turn(Tetra *t) {
	FOR_TETRA {
		pivot(&t->tiles[i]);
	}
}
//A wrapper for a 2D array of fixed width and height
typedef struct Grid {
	unsigned char tiles[WIDTH][HEIGHT];
} Grid;
//Returns the value of the point on the grid.
//Returns 1 if filled and 0 if empty
char getPoint(Grid *g, Point *p) {
	return g->tiles[p->x][p->y];
}
//Clears all tiles on the given row, no bounds checking
void clearRow(Grid *g, short y) {
	for(short x = 0; x < WIDTH; x++) {
		g->tiles[x][y] = 0;
	}
}
//Fills all tiles on the given row, no bounds checking
void fillRow(Grid *g, short y) {
	for(short x = 0; x < WIDTH; x++) {
		g->tiles[x][y] = 1;
	}
}
//Starting at this row and going up, overwrites the current row with the one above it
void descendRow(Grid *g, short start) {
	for(short y = start; y + 1 < HEIGHT; y++) {
		for(short x = 0; x < WIDTH; x++) {
			g->tiles[x][y] = g->tiles[x][y+1];
		}
	}
	clearRow(g, HEIGHT - 1);
}
//Returns whether the row is completely filled, no bounds checking
char rowFull(Grid *g, short y) {
	for(short x = 0; x < WIDTH; x++) {
		if(!g->tiles[x][y]) {
			return 0;
		}
	}
	return 1;
}
//Returns whether the point is in bounds of the grid
char inBoundsPoint(Grid *g, Point *p) {
	return p->x > -1 && p->x < WIDTH && p->y > -1 && p->y < HEIGHT;
}
//Returns whether the tetromino and all of its tiles are in bounds of the grid
char inBounds(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		if(!inBoundsPoint(g, &p) || getPoint(g, &p)) {
			return 0;
		}
	}
	return 1;
}
//Returns whether the tetromino and all of its tiles are in bounds of the grid AND the grid is empty for all the points
char inBoundsOpen(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		if(!inBoundsPoint(g, &p) || getPoint(g, &p)) {
			return 0;
		}
	}
	return 1;
}
//Returns whether the tetromino has reached the bottom or has a filled tile beneath it on the grid.
char land(Grid *g, Tetra *t) {
	FOR_TETRA {
		//Copy the world point from the tetromino
		Point p = getTile(t, i);
		//Land if we reach the bottom
		if(p.y < 1) {
			return 1;
		} else {
			//Land if there's a tile right below us
			p.y--;
			if(inBoundsPoint(g, &p) && getPoint(g, &p)) {
				return 1;
			}
		}
	}
	return 0;
}
//Checks whether the Tetra can turn without leaving bounds
char canTurn(Grid *g, Tetra *t) {
	Tetra turned = *t;
	turn(&turned);
	return inBounds(g, &turned);
}
//Checks whether the Tetra can shift right without leaving bounds
char canShiftRight(Grid *g, Tetra *t) {
	Tetra shifted = *t;
	right(&shifted);
	return inBounds(g, &shifted);
}
//Checks whether the Tetra can shift left without leaving bounds
char canShiftLeft(Grid *g, Tetra *t) {
	Tetra shifted = *t;
	left(&shifted);
	return inBounds(g, &shifted);
}
//Sets the Tetromino's tiles in the grid to solid
void place(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);

		if(inBoundsPoint(g, &p)) {
			g->tiles[p.x][p.y] = 1;
		}
	}
}
//Sets the Tetromino's tiles in the grid to empty
void remove(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		if(inBoundsPoint(g, &p)) {
			g->tiles[p.x][p.y] = 0;
		}
	}
}
unsigned char score;
typedef enum ScreenState { Title, Game, FinalScore } ScreenState;
typedef enum GameState { Init, Play, PlayInterval, RowClear, GameOver, GameOverFlash } GameState;
ScreenState screenState;
void setScreenState(ScreenState next) {
	screenState = next;
	unsigned int a = 0;
	switch(next) {
		case Title:
			score = load(a);
			break;
		case Game:
			screenState = Init;
			break;
		case FinalScore:
			if(score > load(a)) {
				store(0, score);
			}
			break;
	}

};

void UpdateGame() {
	const short standardInterval = 10;
	static Grid g;
	static Tetra t;
	static GameState state = Init;

	static short rowCleared = 0;
	static char rowState = 1;
	static short placed = 0;
	static short fall = 0;

	static short time = 0;

	static char pressed_prev = 0;

	if(--time > 0) {
		return;
	}
	switch(state) {
	case Init:
		t = CreateTetra();
		score = 0;
		time = standardInterval;
		state = Play;
		break;
	case Play:
		remove(&g, &t);			//Remove so that we can move

		char pressed = ~PIN_BUTTONS;
		//char justPressed = pressed & ~pressed_prev;
		switch(pressed) {
		case 1:
			if(canShiftRight(&g, &t)) {
				right(&t);
			}
			break;
		case 2:
			if(canTurn(&g, &t)) {
				turn(&t);
			}
			break;
		case 4:
			if(canShiftLeft(&g, &t)) {
				left(&t);
			}
			break;
		case 5:
			while(!land(&g, &t)) {
				down(&t);
			}
			break;
		}
		pressed_prev = pressed;
		if(land(&g, &t)) {		//See if we stop falling here
			if(inBounds(&g, &t)) {	//We landed in the screen
				place(&g, &t);	//Place in grid
				//Check for rows to clear
				for(unsigned short y = 0; y < HEIGHT; y++) {
					if(rowFull(&g, y)) {
						rowCleared = y;
						rowState = 6;
						state = RowClear;
						score += 1 + y;
						break;
					}
				}
				placed++;
				fall = 0;
				t = CreateTetra();
				if(state != RowClear) {
					time = standardInterval * 5;
					state = PlayInterval;
				} else {
					time = standardInterval;
				}
			} else {
				//We landed above the top of the screen
				//Game over
				state = GameOver;
				rowCleared = 0;
				time = standardInterval;
			}
		} else {
			down(&t);
			place(&g, &t);		//Place in grid so it shows up
			fall++;
			time = standardInterval;
			state = PlayInterval;
		}
		break;
	case PlayInterval:
		time = standardInterval;
		state = Play;
		break;
	case RowClear:
		if(--rowState%2 == 1) {
			fillRow(&g, rowCleared);
		} else {
			clearRow(&g, rowCleared);
		}
		if(rowState > 0) {
			time = standardInterval;
		} else {
			descendRow(&g, rowCleared);

			state = Play;
			for(unsigned short y = 0; y < HEIGHT; y++) {
				if(rowFull(&g, y)) {
					rowCleared = y;
					rowState = 6;
					state = RowClear;
					break;
				}
			}
			if(state != RowClear) {
				time = standardInterval * 3;
			} else {
				time = standardInterval;
			}
		}
		break;
	case GameOver:
		
		if(rowCleared < HEIGHT) {
			clearRow(&g, 0);
			descendRow(&g, 0);
			rowCleared++;
			time = standardInterval;
			state = GameOverFlash;
		} else {
			setScreenState(FinalScore);
		}
		break;
	case GameOverFlash:
		time = standardInterval;
		state = GameOver;
		break;
	}

	if(state == GameOverFlash) {
		//Black screen
		nokia_lcd_clear();
		for(short x = 0; x < WIDTH; x++) {
			for(short y = 0; y < HEIGHT; y++) {
				for(short xi = 0; xi < 4; xi++) {
					for(short yi = 0; yi < 4; yi++) {
						nokia_lcd_set_pixel(y*4 + yi, x*4 + xi, 1);
					}
				}
			}
		}
		nokia_lcd_render();
	} else {
		nokia_lcd_clear();
		for(short x = 0; x < WIDTH; x++) {
			for(short y = 0; y < HEIGHT; y++) {
				for(short xi = 0; xi < 4; xi++) {
					for(short yi = 0; yi < 4; yi++) {
						nokia_lcd_set_pixel(y*4 + yi, x*4 + xi, g.tiles[x][y]);
					}
				}
				
			}
		}
		nokia_lcd_render();
	}
}

int main(void)
{
	/*
	Grid g;
	Tetra t = L((Point) {.x = 2, .y = 18});
	
	signed short i;
	Add: i = 0;
	t = L((Point) {.x = 2, .y = 18});
	while(!land(&g, &t)) {
		down(&t);
		if(canShiftRight(&t))
			right(&t);
		i++;
	}
	//If we're inbounds, then we can place
	if(inBounds(&g, &t)) {
		place(&g, &t);
		goto Add;
	} else {
	//Otherwise, we lost
		int done = 1;
		return;
	}
	*/
	//while(1) UpdateGame();
	/*
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_write_string("IT'S WORKING!",1);
    nokia_lcd_set_cursor(0, 10);
    nokia_lcd_write_string("Nice!", 3);
    nokia_lcd_render();

	TimerSet(1000);
	TimerOn();
	*/
	DDR_BUTTONS = 0xFF;
	PIN_BUTTONS = -1;
	    nokia_lcd_init();
	    nokia_lcd_clear();
		TimerSet(10);
		TimerOn();
	while(1) {
		UpdateGame();
		while(!TimerFlag);
		TimerFlag = 0;
	}

	return;
}

