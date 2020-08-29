#include "drawing.hxx"

#include <stack>
#include <vector>
#include <memory>

#include "sdl2_render.hxx"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>
#include <lodepng.h>
using namespace std;

struct cell_entry {
	int dx;
	int dy;
	int glyph;
	uint32_t fg;
};

struct cell {
	uint32_t bg;	
	vector<cell_entry> entries;
};




static struct state {
	int width = 80;
	int height = 40; 
	std::shared_ptr<tiles> tileset;

	vector<cell> cells;
	int op_layer = 0;
	bool sdl_init = false;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* alias = NULL;
	vector<SDL_Rect> clips;
	bool need_alias = true;
	
	uint32_t fg = 0xFFFFFFFF;
	uint32_t bg = 0x00000000;

	int open();
	void refresh();
	void construct_alias();
	void set_size(int w, int h);
	void put(int x, int y, int c);
	void put_ext(int x, int y, int c, int dx, int dy);

	void set_tile(std::shared_ptr<tiles> tileset);
	std::shared_ptr<tiles> get_tileset();
	void regen_tiles() { need_alias = true; }

	void set_fg(uint32_t fg);
	void set_bg(uint32_t bg);
	void set_layer(int layer);
	int get_layer();
} State;

int state::open() {
	if(!sdl_init) {
			if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
				spdlog::error("Failed to init SDL. %s", SDL_GetError());
				return -2;
			}
		}
		window = SDL_CreateWindow("lyd", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
		if(window == NULL) {
			spdlog::error("Failed to create SDL window.");
			return -1;
		}
		renderer = SDL_CreateRenderer(State.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
		if(renderer == NULL) {
			spdlog::error("Failed to create SDL renderer: %s", SDL_GetError());
			return -1;
		}
		cells.reserve(width * height);	

		return 0;
}

void state::set_tile(std::shared_ptr<tiles> tileset) { this->tileset = tileset; }

void save_texture(SDL_Renderer *ren, SDL_Texture *tex, const char *filename)
{
    SDL_Texture *ren_tex;
    SDL_Surface *surf;
    int st;
    int w;
    int h;
    int format;
    void *pixels;

    pixels  = NULL;
    surf    = NULL;
    ren_tex = NULL;
    format  = SDL_PIXELFORMAT_RGBA32;

    /* Get information about texture we want to save */
    st = SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    if (st != 0) {
        SDL_Log("Failed querying texture: %s\n", SDL_GetError());
        goto cleanup;
    }

    ren_tex = SDL_CreateTexture(ren, format, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!ren_tex) {
        SDL_Log("Failed creating render texture: %s\n", SDL_GetError());
        goto cleanup;
    }

    /*
     * Initialize our canvas, then copy texture to a target whose pixel data we 
     * can access
     */
    st = SDL_SetRenderTarget(ren, ren_tex);
    if (st != 0) {
        SDL_Log("Failed setting render target: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(ren);

    st = SDL_RenderCopy(ren, tex, NULL, NULL);
    if (st != 0) {
        SDL_Log("Failed copying texture data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Create buffer to hold texture data and load it */
    pixels = malloc(w * h * SDL_BYTESPERPIXEL(format));
    if (!pixels) {
        SDL_Log("Failed allocating memory\n");
        goto cleanup;
    }

    st = SDL_RenderReadPixels(ren, NULL, format, pixels, w * SDL_BYTESPERPIXEL(format));
    if (st != 0) {
        SDL_Log("Failed reading pixel data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Copy pixel data over to surface */
    surf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, SDL_BITSPERPIXEL(format), w * SDL_BYTESPERPIXEL(format), format);
    if (!surf) {
        SDL_Log("Failed creating new surface: %s\n", SDL_GetError());
        goto cleanup;
    }
	lodepng::encode("save.png", (unsigned char*)pixels, w, h);	
    /* Save result to an image */
    st = SDL_SaveBMP(surf, filename);
    if (st != 0) {
        SDL_Log("Failed saving image: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_Log("Saved texture as BMP to \"%s\"\n", filename);

cleanup:
    SDL_FreeSurface(surf);
    free(pixels);
    SDL_DestroyTexture(ren_tex);
}


void state::construct_alias() {
	if(tileset == NULL)
		return;
	if(tileset->glyphs.size() == 0) 
		return;
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	int maxX = info.max_texture_width / tileset->tile_width;
	int texW, texH;
	int rows = ceil(tileset->glyphs.size() / maxX);
	if(rows < 1) rows = 1;
	if(rows == 1)
		texW = tileset->glyphs.size() * tileset->tile_width;
	else
		texW = info.max_texture_width;
	texH = rows * tileset->tile_height;
	spdlog::info("Tex W: {}, Tex H: {}", texW, texH);
	if(alias != NULL)
		SDL_DestroyTexture(alias);
	alias = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, texW, texH);
	if(!alias) {
		spdlog::error("Something went wrong: {}", SDL_GetError());
		throw std::runtime_error("OOPS");
	}
	int x = 0;
	int y = 0;
	int idx = 0;
	clips.clear();
	bool color_key = false;
	uint8_t col_r, col_g, col_b, col_a;
	for(auto& gl : tileset->glyphs) {
		SDL_Rect r = { x* tileset->tile_width, y * tileset->tile_height, tileset->tile_width, tileset->tile_height };
		clips.push_back(r);
		SDL_UpdateTexture(alias, &r, &gl[0], tileset->tile_width * 4);
		if(idx == maxX) {
			y++;
			x = 0;
		}
		x++;
		idx++;
	}
	need_alias = false;
	SDL_SetTextureBlendMode(alias, SDL_BLENDMODE_BLEND);
	save_texture(renderer, alias, "alias.bmp");
}

void state::refresh() {
	if(need_alias || alias == NULL)  {
		construct_alias();
	}
	spdlog::info("alias");
	SDL_Rect bg_src = clips[' '];
	int idx = 0;
	for(auto& cell : cells) {
		int x = idx % width;
		int y = idx / width;
		{
			SDL_SetTextureColorMod(alias, (cell.bg >> 24) & 0xFF,
								     	  (cell.bg >> 16) & 0xFF,
										  (cell.bg >> 8) & 0xFF); 
			SDL_Rect dest = { x * tileset->tile_width, y * tileset->tile_height, tileset->tile_width, tileset->tile_height};
			SDL_RenderCopy(renderer, alias, &bg_src, &dest);
		}
		for(auto& entry : cell.entries) {
			SDL_SetTextureColorMod(alias, (entry.fg >> 24) & 0xFF,
								     	  (entry.fg >> 16) & 0xFF,
										  (entry.fg >> 8) & 0xFF); 
			SDL_Rect dest = clips[entry.glyph];
			SDL_Rect dest2 = { x * tileset->tile_width, 
							   y * tileset->tile_height,
							   tileset->tile_width,
							   tileset->tile_height};
			if(entry.dx < 0) {
				int absDx = abs(entry.dx);
				dest.x += absDx;
				dest.w -= absDx;
				dest2.w -= absDx;
			} else {
				dest.w -= entry.dx;
				dest2.x += entry.dx;
				dest2.w -= entry.dx;
			}
			if(entry.dy < 0) {
				int absDy = abs(entry.dy);
				dest.y += absDy;
				dest.h -= absDy;
				dest2.h -= absDy;
			} else {
				dest.h -= entry.dy;
				dest2.y += entry.dy;
				dest2.h -= entry.dy;
			}
			//Dest is actually src.
			SDL_RenderCopy(renderer, alias, &dest, &dest2); 
		}
		idx++;
	}
	SDL_RenderPresent(renderer);
}

void state::set_size(int w, int h) {
	SDL_SetWindowSize(window, w * tileset->tile_width, h * tileset->tile_height);
	width = w;
	height = h;
	cells.resize(w * h);
}

void state::put(int x, int y, int c) {
	put_ext(x, y, c, 0, 0);
}

void state::put_ext(int x, int y, int c, int dx, int dy) {
	if(x < 0 || y < 0 || y >= height || x >= width)	return;
	cell& cel = cells[y * width + x];
	if(cel.entries.size() < op_layer + 1)
		cel.entries.resize(op_layer + 1);
	cell_entry entry = {dx, dy, c, fg};
	cel.entries[op_layer] = entry;
	if(op_layer == 0) 
		cel.bg = bg;
//	cells[y * width + x] = cel;	
}

void state::set_layer(int layer) {
	op_layer = layer;
}

int state::get_layer() {
	return op_layer;
}

void state::set_fg(uint32_t fg) { this->fg = fg; }
void state::set_bg(uint32_t bg) { this->bg = bg; }

std::shared_ptr<tiles> state::get_tileset() {
	return tileset;
}

namespace drawing {
	int open() { return State.open(); }
	void set_size(int w, int h) { State.set_size(w, h);	}
	void set_tileset(std::shared_ptr<tiles> tileset) { State.set_tile(tileset); }
	std::shared_ptr<tiles> get_tileset() { return State.get_tileset(); }	
	void set_fg(uint32_t fg) { State.set_fg(fg); }
	void set_bg(uint32_t bg) { State.set_bg(bg); }
	void set_layer(int layer) { State.set_layer(layer); }
	int get_layer() { return State.get_layer(); }

	void put(int x, int y, int c) {	State.put(x, y, c);	}
	void put_ext(int x, int y, int c, int dx, int dy) {	State.put_ext(x, y, c, dx, dy);	}

	void refresh() { State.refresh(); }
	void tileset_changed() { State.regen_tiles(); }

	int tileset_from_file(const std::string file) {
		tiles* tileset = new tiles();
		std::vector<unsigned char> out;
		unsigned w, h, error;
		error = lodepng::decode(out, w, h, file, LCT_RGBA);
		if(error) {
			spdlog::error("Error decoding png: {}", lodepng_error_text(error));
			return -1;
		}
		tileset->tile_height = h / 16;
		tileset->tile_width = w / 16;
		uint8_t r, g, b, a;
		bool color_key =false;
		a = out[3];
		if(a != 0) {
			r = out[0];
			g = out[1];
			b = out[2];
			color_key = true;
		}
		for(int y = 0; y < 16; y++)
		for(int x = 0; x < 16; x++) {
			std::vector<uint8_t> img;
			img.resize(4 * tileset->tile_height * tileset->tile_width);
			for(int i = 0; i < tileset->tile_height; i++) {
				int img_off = i * tileset->tile_width * 4; 
				int pix_off = ((y * tileset->tile_height) * (w * 4) + (x * tileset->tile_width * 4)) + (i * (w * 4)); 
				memcpy(&img[img_off], &out[pix_off], tileset->tile_width * 4);
			}
			if(color_key) {
				for(int i = 0; i < img.size(); i+= 4) {
					if(img[i] == r && img[i+1] == g && img[i+2] == b) {
						img[i+3] = 0;
					}
				}
			}
			tileset->glyphs.push_back(img);
		}
		drawing::set_tileset(std::shared_ptr<tiles>(tileset));
		drawing::tileset_changed();
		return 0;
	}
}
