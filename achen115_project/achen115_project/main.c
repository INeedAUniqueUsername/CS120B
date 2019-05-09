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
typedef struct Point {
	signed short x, y;
} Point;
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
		if(!inBoundsPoint(g, &t->tiles[i])) {
			return 1;
		}
	}
	return 0;
}
char get(Grid *g, Point *p) {
	return g->tiles[p->x][p->y];
}
char land(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		//Land if we reach the bottom
		if(!p.y) {
			return 1;
		} else {
			//Land if there's a tile right below us
			p.y--;
			if(get(g, &p)) {
				return 1;
			}
		}
	}
	return 0;
}
void place(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);

		if(inBoundsPoint(&g, &p)) {
			g->tiles[p.x][p.y] = 1;
		}
	}
}
void remove(Grid *g, Tetra *t) {
	FOR_TETRA {
		Point p = getTile(t, i);
		if(inBoundsPoint(&g, &p)) {
			g->tiles[p.x][p.y] = 0;
		}
	}
}
typedef enum GameState { Init, Play, RowClear } GameState;
void UpdateGame() {
	static Grid g;
	static Tetra t;
	static GameState state = Init;

	static short rowCleared = 0;
	static char rowVisible = 1;

	static short time = 0;

	if(--time > 0) {
		return;
	}
	switch(state) {
	case Init:
		t = (Tetra) {.pos = (Point) { .x = WIDTH/2, .y = HEIGHT}, .tiles = {n, c, s, se}};
		time = 1000;
	case Play:
		remove(&g, &t);
		if(land(&g, &t)) {
			if(inBounds(&g, &t)) {
				place(&g, &t);
				for(unsigned short y = 0; y < HEIGHT; y++) {
					if(rowFull(&g, y)) {
						rowCleared = y;
						state = RowClear;
					}
				}
				} else {
				//Game over
			}
		} else {
			down(&t);
			place(&g, &t);
		}
		time = 500;
		break;
	case RowClear:
		if(rowVisible) {
			fillRow(&g, rowCleared);
		} else {
			clearRow(&g, rowCleared);
		}
		rowVisible = !rowVisible;
		time = 200;
		break;
	}
}

int main(void)
{
	Grid g;
	Tetra t = L((Point) {.x = 2, .y = 2});

	signed short i = 0;
	while(!land(&g, &t)) {
		down(&t);
		i++;
	}
	place(&g, &t);
	

	return;
}

