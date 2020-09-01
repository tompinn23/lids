#pragma once

#include "terminal.hxx"

#include <cstdint>	
// May need refractoring to SDL_Texture.

class Console {
public:
	Console(int x, int y, int width, int height);
	void put(int x, int y, int code);
	void put_ex(int x, int y, int dx, int dy, int code);
	void box(int thick);
	void draw();
private:
	terminal::Layer term_layer;
	uint32_t fg;
	uint32_t bg;
	int x;
	int y;
	int width;
	int height;
};
