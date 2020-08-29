#pragma once

#include <vector>
#include <SDL2/SDL.h>


struct tiles;

class sdl2_renderer {
public:
	sdl2_renderer(tiles* tileset) : tileset(tileset) {}
	SDL_Texture* get_alias_tex(int c);
	SDL_Rect get_clipping_box(int c);
private:
	tiles* tileset;
	int last_tile_sz;
	std::vector<int> alias_lim;
	std::vector<SDL_Texture*> aliases;

};
