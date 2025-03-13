#ifndef CUSTOM_CHESS_CHARS_H
#define CUSTOM_CHESS_CHARS_H

#include <Arduino.h>

// TODO: invert the colors on these lol

byte knight[8] = {
	0b11101,
	0b10000,
	0b00000,
	0b00000,
	0b11100,
	0b11000,
	0b10000,
	0b00000
};

byte rook[8] = {
	0b01010,
	0b01010,
	0b00000,
	0b10001,
	0b10001,
	0b10001,
	0b00000,
	0b00000
};

byte bishop[8] = {
	0b11011,
	0b11011,
	0b10001,
	0b10001,
	0b11011,
	0b11011,
	0b10001,
	0b00000
};

byte queen[8] = {
	0b11011,
	0b01010,
	0b00000,
	0b10001,
	0b11011,
	0b11011,
	0b10001,
	0b00000
};


#endif 
