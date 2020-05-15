#pragma once

typedef struct {
	// keyboard
	bool forward, back, left, right, up, down;
	bool turnleft, turnright, turnup, turndown;
	// mouse
	signed int dx, dy;
} InputState;

void init_input();
void read_input(InputState* state);
