#pragma once

#include "terminal.hxx"

#include <fmt/printf.h>
#include <cstdint>	
// May need refractoring to SDL_Texture.

class Console {
public:
	Console(int width, int height);
	void put(int x, int y, int code);
	void put_ex(int x, int y, int dx, int dy, int code);
	void box(bool thick);
	void draw(int x, int y);
	void clear(int x, int y);
	void clear();
	void set_fg(uint32_t fg) { this->fg = fg; }
	void set_bg(uint32_t bg) { this->bg = bg; }
	template<typename... Args>
	void printf(int x, int y, const char* format, const Args&... args) {
		con_printf(x, y, 0, format, fmt::make_printf_args(args...));
	}
	template<typename... Args>
	void printf(int x, int y, int sz_limit, const char* format, const Args&... args) {
		con_printf(x, y, sz_limit, format, fmt::make_printf_args(args...));
	}
	template<typename... Args>
	int sizef(const char* format, const Args&... args) {
		return con_sizef(format, fmt::make_printf_args(args...));
	}
private:
	void con_printf(int x, int y, int sz_limit, const char* format, fmt::printf_args args);
	int con_sizef(const char* format, fmt::printf_args args);
	terminal::Layer term_layer;
	uint32_t fg = 0xFFFFFFFF;
	uint32_t bg = 0x000000FF;
	int x;
	int y;
	int width;
	int height;
};
