/*
 * achen115_project.c
 *
 * Created: 5/8/2019 3:24:34 PM
 * Author : Alex
 */ 

#include <avr/io.h>

#define FOR_TETRA for(unsigned short i = 0; i < 4; i++)
#define WIDTH 8
#define HEIGHT 16

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
//Rotates a point CW around the origin
void pivot(Point* p) {
	signed short x = p->x;
	p->x = -p->y;
	p->y = x;
}

typedef struct Tetra {
	Point pos;
	Point tiles[4];
} Tetra;
#define nn (Point) { .x = 0, .y = 2 }
#define n (Point) { .x = 0, .y = 1 }
#define ne (Point) { .x = 1, .y = 1 }
#define e (Point) { .x = 1, .y = 0 }
#define se (Point) { .x = 1, .y = -1 }
#define s (Point) { .x = 0, .y = -1 }
#define sw (Point) { .x = -1, .y = -1 }
#define w (Point) { .x = -1, .y = 0 }
#define nw (Point) { .x = -1, .y = 1 }
#define c (Point) { .x = 0, .y = 0 }
Tetra L(Point pos) { return (Tetra) {.pos = pos, .tiles = {n, c, s, se}}; }
Tetra Z(Point pos) { return (Tetra) {.pos = pos, .tiles = {w, c, e, ne}}; }
Tetra I(Point pos) { return (Tetra) {.pos = pos, .tiles = {s, c, n, nn}}; }
Tetra O(Point pos) { return (Tetra) {.pos = pos, .tiles = {s, c, e, se}}; }
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
Point getTile(Tetra *t, unsigned short index) {
	return add(&t->pos, &t->tiles[index]);
}
void down(Tetra *t) {
	t->pos.y--;
}
//When moving/rotating, make sure that the tetromino is still in bounds and in open space
void right(Tetra *t) {
	t->pos.x++;
}
void left(Tetra *t) {
	t->pos.x--;
}
void mirror(Tetra* t) {
	for(unsigned short i = 0; i < 4; i++) {
		t->tiles->x = -t->tiles->x;
	}
}
void turn(Tetra *t) {
	FOR_TETRA {
		pivot(&t->tiles[i]);
	}
}
typedef struct Grid {
	unsigned char tiles[WIDTH][HEIGHT];
} Grid;
char getPoint(Grid *g, Point *p) {
	return g->tiles[p->x][p->y];
}
void clearRow(Grid *g, short y) {
	for(short x = 0; x > WIDTH; x++) {
		g->tiles[x][y] = 0;
	}
}
void fillRow(Grid *g, short y) {
	for(short x = 0; x > WIDTH; x++) {
		g->tiles[x][y] = 1;
	}
}
void descendRow(Grid *g, short start) {
	for(short y = start; y + 1 < HEIGHT; y++) {
		for(short x = 0; x < WIDTH; x++) {
			g->tiles[x][y] = g->tiles[x][y+1];
		}
	}
	clearRow(g, HEIGHT - 1);
}
char rowFull(Grid *g, short y) {
	for(short x = 0; x < WIDTH; x++) {
		if(!g->tiles[x][y]) {
			return 0;
		}
	}
	return 1;
}
char inBoundsPoint(Grid *g, Point *p) {
	return p->x > -1 && p->x < WIDTH && p->y > -1 && p->y < HEIGHT;
}
char inBounds(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		if(!inBoundsPoint(g, &p) || getPoint(g, &p)) {
			return 0;
		}
	}
	return 1;
}
char inBoundsOpen(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		if(!inBoundsPoint(g, &p) || getPoint(g, &p)) {
			return 0;
		}
	}
	return 1;
}
char land(Grid *g, Tetra *t) {
	FOR_TETRA {
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
char canTurn(Grid *g, Tetra *t) {
	Tetra turned = *t;
	turn(&turned);
	return inBounds(g, &turned);
}
char canShiftRight(Grid *g, Tetra *t) {
	Tetra shifted = *t;
	right(&shifted);
	return inBounds(g, &shifted);
}
char canShiftLeft(Grid *g, Tetra *t) {
	Tetra shifted = *t;
	left(&shifted);
	return inBounds(g, &shifted);
}
void place(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);

		if(inBoundsPoint(g, &p)) {
			g->tiles[p.x][p.y] = 1;
		}
	}
}
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
typedef enum GameState { Init, Play, RowClear, GameOver } GameState;
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
		time = 1000;
		state = Play;
		return;
	case Play:
		remove(&g, &t);			//Remove so that we can move

		char pressed = ~PINA;
		char justPressed = pressed & ~pressed_prev;
		switch(justPressed) {
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
		if(land(&g, &t)) {		//See if we stop falling here
			if(inBounds(&g, &t)) {	//We landed in the screen
				place(&g, &t);	//Place in grid
				//Check for rows to clear
				for(unsigned short y = 0; y < HEIGHT; y++) {
					if(rowFull(&g, y)) {
						rowCleared = y;
						rowState = 4;
						state = RowClear;
						time = 250;
						score += 1 + y;
						return;
					}
				}
				placed++;
				fall = 0;
				t = CreateTetra();
				time = 500;
			} else {
				//We landed above the top of the screen
				//Game over
				state = GameOver;
				rowCleared = 0;
				time = 1000;
			}
		} else {
			down(&t);
			place(&g, &t);		//Place in grid so it shows up
			fall++;
			time = 500;
		}
		return;
	case RowClear:
		if(rowState%2) {
			fillRow(&g, rowCleared);
		} else {
			clearRow(&g, rowCleared);
		}
		if(rowState > 0) {
			rowState--;
			time = 250;
		} else {
			descendRow(&g, rowCleared);

			for(unsigned short y = 0; y < HEIGHT; y++) {
				if(rowFull(&g, y)) {
					rowCleared = y;
					rowState = 4;
					state = RowClear;
					time = 250;
					return;
				}
			}

			state = Play;
			time = 500;
		}
		return;
	case GameOver:
		if(rowCleared < HEIGHT) {
			clearRow(&g, 0);
			descendRow(&g, 0);
			rowCleared++;
			time = 500;
		} else {
			setScreenState(FinalScore);
		}
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
	while(1) UpdateGame();

	return;
}

