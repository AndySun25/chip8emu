#include <stdbool.h>
#include <stdio.h>

#pragma once

#define getNibble(val, i) (((val) >> ((i) * 4) & 0xF))
#define GFX_W 64
#define GFX_H 32

unsigned char V[16];
unsigned char memory[4096];

unsigned char gfx[GFX_W * GFX_H];

unsigned short opcode;
unsigned short pc;
unsigned short I;

unsigned char delay_timer;
unsigned char sound_timer;

unsigned short stack[16];
unsigned short sp;

unsigned char key[0xF];

bool draw_flag;

void resetKeys(void);
void cycle(void);
void reset(void);
bool load(const char * filename);
