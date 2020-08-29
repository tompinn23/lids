#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <memory>

typedef uint8_t* (*tile_cb)(int, void* udata);


struct tiles {
	std::vector<std::vector<uint8_t>> glyphs;
	int tile_height;
	int tile_width;
};


namespace drawing {
	int open();
	void set_size(int w, int h);
	void set_tileset(std::shared_ptr<tiles> tileset);
	std::shared_ptr<tiles> get_tileset();

	void set_fg(uint32_t fg);
	void set_bg(uint32_t bg);
	void set_layer(int layer);
	int get_layer();

	void put(int x, int y, int c);
	void put_ext(int x, int y, int c, int dx, int dy);
	
	void tileset_changed();
	void refresh();
	
	int tileset_from_file(const std::string file);
}
