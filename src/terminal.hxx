#pragma once

#include "tileset.hxx"

#include <vector>
#include <cstdint>
#include <memory>

#include <SDL2/SDL.h>


namespace terminal {
	constexpr uint32_t to_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return (r << 24) + (g << 16) + (b << 8) + a;
	}

	constexpr uint8_t get_r(uint32_t rgba) {
		return rgba >> 24 & 0xFF;
	}

	constexpr uint8_t get_g(uint32_t rgba) {
		return rgba >> 16 & 0xFF;
	}

	constexpr uint8_t get_b(uint32_t rgba) {
		return rgba >> 8 & 0xFF;
	}

	constexpr uint8_t get_a(uint32_t rgba) {
		return rgba & 0xFF;
	}
struct cell_entry {
	int dx;
	int dy;
	int glyph;
	uint32_t fg;
};

struct cell {
	uint32_t bg;
	std::vector<cell_entry> entries;
};

struct Layer {
	std::vector<cell> cells;
	void clear(uint32_t bg = 0);
};

enum terminal_err {
	TERM_SDL_INIT_ERR = -1,
	TERM_SDL_CREATE_ERR = -2,
};

enum {
	TERM_LAYER,
	TERM_COMPOSITION,
};

class Terminal {
public:
	Terminal();
	Terminal(int width, int height, std::shared_ptr<Tileset> tileset);
	~Terminal();

	int open();
	int close();
	void size(int width, int height);
	void set_tileset(std::shared_ptr<Tileset> tileset) { this->tileset = tileset; } 
	void set_fg(uint32_t fg);
	void set_bg(uint32_t bg);
	void composition(bool comp);
	void layer(int layer);

	void clear();
	void clear_area(int x, int y, int w, int h);
	void crop(int x, int y, int w, int h);

	void refresh();
	
	void put(int x, int y, int code);
	void put_ex(int x, int y, int dx, int dy, int code);
	int pick(int x, int y, int idx);
	uint32_t pick_colour(int x, int y, int idx);
	uint32_t pick_bkcolour(int x, int y);
	int state(int slot);

	int has_input();
	int read();
	int peek();
	
	static void change_tile(void* udata);
	int render_entry(int x, int y,cell_entry& ent);
	void raw_put(int x, int y, cell c);
private:
	void tileset_changed();
	void construct_alias();
	std::shared_ptr<Tileset> tileset;
	int width;
	int height;
	
	std::vector<Layer> layers;
	int operating_layer = 0;
	bool comp;

	bool sdl_init = false;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* alias;
	std::vector<SDL_Rect> clips;
	bool need_alias = false;

	uint32_t fg = 0xFFFFFFFF;
	uint32_t bg = 0;
};

	int open();
	int close();
	void size(int width, int height);
	void set_tileset(std::shared_ptr<Tileset> tileset);
	void set_fg(uint32_t fg);
	void set_bg(uint32_t bg);
	void composition(bool comp);
	void layer(int layer);

	void clear();
	void clear_area(int x, int y, int w, int h);
	void crop(int x, int y, int w, int h);

	void refresh();
	
	void put(int x, int y, int code);
	void put_ex(int x, int y, int dx, int dy, int code);
	int pick(int x, int y, int idx);
	uint32_t pick_colour(int x, int y, int idx);
	uint32_t pick_bkcolour(int x, int y);
	int state(int slot);
	void raw_put(int x, int y, cell c);
	int has_input();
	int read();
	int peek();


}
