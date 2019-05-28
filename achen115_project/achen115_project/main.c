/*
 * achen115_project.c
 *
 * Created: 5/8/2019 3:24:34 PM
 * Author : Alex
 */ 

#include <avr/eeprom.h>
#include <avr/io.h>
#include "nokia5110.c"
#include "timer.h"

#define FOR_TETRA for(unsigned short i = 0; i < 4; i++)
#define WIDTH 12
#define HEIGHT 21
#define DDR_BUTTONS DDRD
#define PIN_BUTTONS PIND
#define PORT_MUSIC PIND
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
void pivotCCW(Point* p) {
	signed short x = p->x;
	p->x = -p->y;
	p->y = x;
}

// Rotates a point 90 degrees CW around the origin
// We store the Tetromino tiles as relative positions
// So we can use a neat math trick to do this
// ty = y
// y = -x
// x = ty
// ( 1, 0)  ( 0,-1)  (-1, 0)  ( 0, 1)
//   ...      ...      ...      .+.
//   ..+      ...      +..      ...
//   ...      .+.      ...      ...
void pivotCW(Point* p) {
	signed short y = p->y;
	p->y = -p->x;
	p->x = y;
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
Tetra Z(Point pos) { return (Tetra) {.pos = pos, .tiles = {w, c, n, ne}}; }
Tetra I(Point pos) { return (Tetra) {.pos = pos, .tiles = {s, c, n, nn}}; }
Tetra O(Point pos) { return (Tetra) {.pos = pos, .tiles = {s, c, e, se}}; }
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
//Flips the tetromino tiles across X without bounds checking
void mirror(Tetra* t) {
	for(unsigned short i = 0; i < 4; i++) {
		t->tiles[i].x = -t->tiles[i].x;
	}
}
//Rotates the tetromino tiles around the center without bounds checking
void turnCCW(Tetra *t) {
	FOR_TETRA {
		pivotCCW(&t->tiles[i]);
	}
}
//Rotates the tetromino tiles around the center without bounds checking
void turnCW(Tetra *t) {
	FOR_TETRA {
		pivotCW(&t->tiles[i]);
	}
}
//Creates a random tetromino
Tetra CreateTetra() {
	char x = WIDTH/2 + rand()%8 - 4;
	Point pos = {.x = x, .y = HEIGHT + 2};
	Tetra t;
	switch(rand()%4) {
		case 0: t = L(pos); break;
		case 1: t = Z(pos); break;
		case 2: t = I(pos); break;
		default:
		case 3: t = O(pos); break;
	}
	switch(rand()%3) {
		case 0:
		turnCCW(&t);
		break;
		case 1:
		turnCW(&t);
		break;
		case 2:
		default:
		break;
	}
	if(rand()%2) {
		mirror(&t);
	}
	return t;
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
void clear(Grid *g) {
	for(short x = 0; x < WIDTH; x++) {
		for(short y = 0; y < HEIGHT; y++) {
			g->tiles[x][y] = 0;
		}
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
//Returns whether the row is completely filled, no bounds checking
char rowEmpty(Grid *g, short y) {
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
//Mirrors the tetromino, reverting the operation if it goes out of bounds
void tryMirror(Grid *g, Tetra *t) {
	//Try it
	mirror(t);
	//If it works, we're done
	if(inBoundsOpen(g, t)) {
		return;
	}
	//Otherwise, revert
	mirror(t);
}
//Turns the Tetromino 90 degrees CCW, reverting the operation if it goes out of bounds
void tryTurnCCW(Grid *g, Tetra *t) {
	//Try it
	turnCCW(t);
	//If it works, we're done
	if(inBoundsOpen(g, t)) {
		return;
	}
	//Otherwise, revert
	turnCW(t);
}
//Turns the Tetromino 90 degrees CW, reverting the operation if it goes out of bounds
void tryTurnCW(Grid *g, Tetra *t) {
	//Try it
	turnCW(t);
	//If it works, we're done
	if(inBoundsOpen(g, t)) {
		return;
	}
	//Otherwise, revert
	turnCCW(t);
}
//Shifts the Tetromino right, reverting the operation if it goes out of bounds
void tryShiftRight(Grid *g, Tetra *t) {
	right(t); //Try it
	//See if it worked
	if(inBoundsOpen(g, t)) {
		return;
	}
	left(t);	//Otherwise go back
}
//Shifts the Tetromino right, reverting the operation if it goes out of bounds
void tryShiftLeft(Grid *g, Tetra *t) {
	left(t); //Try it
	//See if it worked
	if(inBoundsOpen(g, t)) {
		return;
	}
	right(t);	//Otherwise go back
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
typedef enum ScreenState { Title, Game, FinalScore } ScreenState;
ScreenState screenState;
typedef enum GameState { Init, Play, PlayInterval, RowClear, GameOver, GameOverFlash } GameState;
GameState gameState;
unsigned char score = 0;
unsigned char highScore = 0;
unsigned char ADDR_SCORE = 0xFF;
unsigned char rnd = 0;
short getLength(short sh) {
	unsigned char length = 1;
	//Count the number in case it is 0
	if(sh == 0) {
		length++;
	} else {
		while(sh > 0) {
			length++;
			sh /= 10;
		}
	}
	return length;
}
void getString(short sh, char* str, short length) {
	length--;
	str[length] = 0;
	//Print the number in case it is 0
	if(sh == 0) {
		length--;
		str[length] = '0';
	} else {
		while(sh > 0) {
			length--;
			str[length] = '0' + (sh%10);
			sh /= 10;
		}
	}
	return;
}
typedef enum SoundState { MusicTitle = 1, MusicTypeA = 2, MusicGameOver = 3, MusicHighScore = 4 } SoundState;
void UpdateTitle() {
	
	nokia_lcd_clear();
	nokia_lcd_write_string("Tetris!", 1);
	nokia_lcd_set_cursor(0, 10);
	nokia_lcd_write_string("High score: ", 1);
	nokia_lcd_set_cursor(0, 20);
	
	unsigned short highScoreLength = getLength(highScore);
	char highScoreString[highScoreLength];
	getString(highScore, highScoreString, highScoreLength);
	nokia_lcd_write_string(highScoreString, 1);

	nokia_lcd_set_cursor(0, 30);
	nokia_lcd_write_string("Random seed:", 1);

	unsigned short rndLength = getLength(rnd);
	char rndString[rndLength];
	getString(rnd, rndString, rndLength);

	nokia_lcd_set_cursor(0, 40);
	nokia_lcd_write_string(rndString, 1);

	nokia_lcd_render();


	char pressed = ~PIN_BUTTONS & 7;
	//static char pressed_prev = 0;
	//char justPressed = pressed & ~pressed_prev;
	switch(pressed) {
		case 1:	//Right
			rnd++;
			break;
		case 2:	//Middle
			screenState = Game;
			gameState = Init;
			break;
		case 3:	//Middle + Right
			break;
		case 4:	//Left
			rnd--;
			break;
		case 5:	//Left + Right
			srand(rnd);
			rnd = rand();
			break;
		case 6:	//Left + Middle
			
			break;
		case 7:	//Left + Middle + Right
			highScore = 0;
			eeprom_write_byte(&ADDR_SCORE, highScore);
			break;
	}
	//pressed_prev = pressed;
}
void UpdateFinalScore() {
	
	nokia_lcd_clear();
	nokia_lcd_write_string("Your score: ", 1);
	nokia_lcd_set_cursor(0, 10);

	unsigned short scoreLength = getLength(score);
	char scoreString[scoreLength];
	getString(score, scoreString, scoreLength);
	nokia_lcd_write_string(scoreString, 1);

	nokia_lcd_set_cursor(0, 20);
	nokia_lcd_write_string("High score: ", 1);
	nokia_lcd_set_cursor(0, 30);
	
	unsigned short highScoreLength = getLength(highScore);
	char highScoreString[highScoreLength];
	getString(highScore, highScoreString, highScoreLength);
	nokia_lcd_write_string(highScoreString, 1);

	nokia_lcd_set_cursor(0, 40);
	if(score > highScore) {
		nokia_lcd_write_string("New high score!", 1);	
	}

	nokia_lcd_render();


	char pressed = ~PIN_BUTTONS & 7;
	static char pressed_prev = 0;
	char justPressed = pressed & ~pressed_prev;
	switch(justPressed) {
		case 0: break;
		default:
			highScore = highScore > score ? highScore : score;
			eeprom_write_byte(&ADDR_SCORE, highScore);
			screenState = Title;
			break;
	}
	pressed_prev = pressed;
}
//Draws a world tile as a 4x4 block on a vertical screen
void drawTile(short x, short y, short fill) {
	for(short xi = 0; xi < 4; xi++) {
		for(short yi = 0; yi < 4; yi++) {
			nokia_lcd_set_pixel(y*4 + yi, x*4 + xi, fill);
		}
	}
}
void UpdateGame() {
	const short standardInterval = 10;
	static Grid g;
	static Tetra t;
	static GameState gameState = Init;

	static char pressed_prev = 0;
	static char hard_drop = 0;

	static short rowCleared = 0;
	static char rowState = 0;
	static short placed = 0;
	static short fall = 0;

	static short time = 0;

	if(--time > 0) {
		return;
	}
	switch(gameState) {
	case Init: {
		clear(&g);
		t = CreateTetra();

		pressed_prev = 0;
		hard_drop = 0;

		rowCleared = 0;
		rowState = 0;
		placed = 0;
		fall = 0;

		time = standardInterval;
		//Reset the RNG
		srand(rnd);
		//Reset the score
		score = 0;
		gameState = Play;
		//To do: Reset all static variables so that they don't carry over from previous games
		break;
	} case Play: {
		//remove(&g, &t);			//Remove so that we can move

		char pressed = ~PIN_BUTTONS & 7;
		//char justPressed = pressed & ~pressed_prev;
		switch(pressed) {
		case 1:	//Right
			tryShiftRight(&g, &t);
			break;
		case 2:	//Middle
			tryMirror(&g, &t);
			break;
		case 3:	//Middle + Right
			tryTurnCW(&g, &t);
			break;
		case 4:	//Left
			tryShiftLeft(&g, &t);
			break;
		case 5:	//Left + Right
			while(!land(&g, &t)) {
				down(&t);
			}
			break;
		case 6:	//Left + Middle
			tryTurnCCW(&g, &t);
			break;
		case 7:	//Left + Middle + Right
			break;
		}
		pressed_prev = pressed;
		if(pressed_prev == pressed && pressed == 5) {
			hard_drop++;
		} else {
			hard_drop = 0;
		}


		if(land(&g, &t)) {		//See if we stop falling here
			if(inBoundsOpen(&g, &t)) {	//We landed in the screen
				place(&g, &t);	//Place in grid
				gameState = PlayInterval;
				//Check for rows to clear
				for(unsigned short y = 0; y < HEIGHT; y++) {
					if(rowFull(&g, y)) {
						rowCleared = y;
						rowState = 6;
						gameState = RowClear;
						score += 1 + y;
						break;
					}
				}
				placed++;
				fall = 0;
				t = CreateTetra();
				if(gameState != RowClear) {
					if(hard_drop > 12) {
						time = 0;
					}
					else if(hard_drop > 4) {
						time = standardInterval / 2;
					} else {
						time = standardInterval * 5;
					}
					
				} else {
					time = standardInterval;
				}
			} else {
				//We landed above the top of the screen
				//Game over
				gameState = GameOver;
				rowCleared = 0;
				time = standardInterval;
			}
		} else {
			down(&t);
			//place(&g, &t);
			fall++;
			time = standardInterval;
			gameState = PlayInterval;
		}
		break;
	} case PlayInterval:
		time = standardInterval;
		gameState = Play;
		break;
	case RowClear:
		if(--rowState%2 == 1) {
			clearRow(&g, rowCleared);
		} else {
			fillRow(&g, rowCleared);
		}
		if(rowState > 0) {
			time = standardInterval;
		} else {
			descendRow(&g, rowCleared);

			gameState = Play;
			for(unsigned short y = 0; y < HEIGHT; y++) {
				if(rowFull(&g, y)) {
					rowCleared = y;
					rowState = 6;
					gameState = RowClear;
					break;
				}
			}
			if(gameState != RowClear) {
				time = standardInterval * 3;
			} else {
				time = standardInterval;
			}
		}
		break;
	case GameOver:
		if(rowCleared < HEIGHT && !rowEmpty(&g, rowCleared)) {
			clearRow(&g, 0);
			descendRow(&g, 0);
			rowCleared++;
			time = standardInterval;
			gameState = GameOverFlash;
		} else {
			time = standardInterval;
			screenState = FinalScore;

			gameState = Init;				//For some reason, future games immediately go to the Game Over screen unless the state is set here
		}
		break;
	case GameOverFlash:
		time = standardInterval;
		gameState = GameOver;
		break;
	}
	nokia_lcd_clear();
	if(gameState == GameOverFlash) {
		//Black screen
		for(short x = 0; x < WIDTH; x++) {
			for(short y = 0; y < HEIGHT; y++) {
				drawTile(x, y, 1);
			}
		}
	} else if(gameState == Play) {
		for(short x = 0; x < WIDTH; x++) {
			for(short y = 0; y < HEIGHT; y++) {
				drawTile(x, y, g.tiles[x][y]);
			}
		}
		for(short i = 0; i < 4; i++) {
			Point p = getTile(&t, i);
			if(p.y < HEIGHT) {
				drawTile(p.x, p.y, 1);
			}
		}
		Tetra landed = t;
		while(!land(&g, &landed)) {
			down(&landed);
		}
		for(short i = 0; i < 4; i++) {
			Point p = getTile(&landed, i);
			if(p.y < HEIGHT) {
				drawTile(p.x, p.y, 1);
			}
		}
	} else if(gameState == PlayInterval) {
		for(short x = 0; x < WIDTH; x++) {
			for(short y = 0; y < HEIGHT; y++) {
				drawTile(x, y, g.tiles[x][y]);
			}
		}
		for(short i = 0; i < 4; i++) {
			Point p = getTile(&t, i);
			if(p.y < HEIGHT) {
				drawTile(p.x, p.y, 1);
			}
		}
	} else {
		//Otherwise just draw the grid
		for(short x = 0; x < WIDTH; x++) {
			for(short y = 0; y < HEIGHT; y++) {
				drawTile(x, y, g.tiles[x][y]);
			}
		}
	}
	nokia_lcd_render();
}
void UpdateState() {
	switch(screenState) {
	case Title: UpdateTitle();
		PORT_MUSIC = (MusicTitle) << 3;
		break;
	case Game: UpdateGame();
		PORT_MUSIC = (MusicTypeA) << 3;
		break;
	case FinalScore: UpdateFinalScore();
		PORT_MUSIC = (MusicHighScore) << 3;
		break;
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
	DDR_BUTTONS = ~0x07;
	PIN_BUTTONS = -1;
	PORT_MUSIC = 0;
	nokia_lcd_init();
	nokia_lcd_clear();
	TimerSet(10);
	TimerOn();

	screenState = Title;
	highScore = eeprom_read_byte(&ADDR_SCORE);
	while(1) {
		UpdateState();
		while(!TimerFlag);
		TimerFlag = 0;
	}

	return 0;
}

