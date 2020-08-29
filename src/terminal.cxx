#include "terminal.hxx"

#include <spdlog/spdlog.h>


namespace terminal {
	void Layer::clear(uint32_t bg) {
		for(auto &a : cells) {
			if(a.entries.size() > 0)
				a.entries.clear();
			a.bg = bg;
		}
	}
	
	void Terminal::change_tile(void* udata) {
		((Terminal*)udata)->tileset_changed();
	}
	
	void Terminal::tileset_changed() {
		need_alias = true;
	}

	Terminal::Terminal() {
		width = 0;
		height = 0;
		layers.resize(8);
	}

	Terminal::Terminal(int width, int height, std::shared_ptr<Tileset> tileset) {
		this->tileset = tileset;
		tileset->add_observer(Terminal::change_tile, this);
		need_alias = true;
		this->width = width;
		this->height = height;
		layers.resize(8);
		for(auto& l : layers) {
			l.cells.resize(width * height, {0, {}});
		}
	}

	Terminal::~Terminal() {
		if(window != NULL || renderer != NULL || alias != NULL) {
			spdlog::warn("Forgot to call terminal::close()");	
		}
	}

	int Terminal::open() {
		if(sdl_init == false) {
			if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
				spdlog::error("Failed to initialise SDL: {}", SDL_GetError());
				return TERM_SDL_INIT_ERR;
			}
			sdl_init = true;
		}
		window = SDL_CreateWindow("lyds", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600 , 0);
		if(window == NULL) {
			spdlog::error("Failed to create window: {}", SDL_GetError());
			return TERM_SDL_CREATE_ERR;
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if(renderer == NULL) {
			spdlog::error("Failed to create renderer: {}", SDL_GetError());
			return TERM_SDL_CREATE_ERR;
		}
		return 0;
	}

	int Terminal::close() {
		if(sdl_init == false) {
			spdlog::warn("close() called before open() or double close()");
			return 0;
		} else {
			if(alias) {
				SDL_DestroyTexture(alias);
				alias = NULL;
			}
			if(renderer) {
				SDL_DestroyRenderer(renderer);
				renderer = NULL;
			}
			if(window) {
				SDL_DestroyWindow(window);
				window = NULL;
			}
		}
		return 0;
	}
	void Terminal::size(int width, int height) {
		if(!tileset) {
			spdlog::warn("No active tileset size won't work correctly.");
			SDL_SetWindowSize(window, width, height);
			this->width = width;
			this->height = height;
			return;
		}
		SDL_SetWindowSize(window, width * tileset->width(), height * tileset->height());
		this->width = width;
		this->height = height;
		cell c = {0, std::vector<cell_entry>() };
		for(auto& l : layers) {
			l.cells.resize(width * height, c);
		}
	}
	void Terminal::set_fg(uint32_t fg) { this->fg = fg; }
	void Terminal::set_bg(uint32_t bg) { this->bg = bg; }	

	void Terminal::composition(bool comp) { this->comp = comp; }
	void Terminal::layer(int layer) { this->operating_layer = layer; }
	
	void Terminal::clear() {
		for(auto &l : layers) {
			l.clear(bg);
		}
	}

	void Terminal::clear_area(int x, int y, int w, int h) {
		if(x < 0) x = 0;
		if(y < 0) y = 0;
		for(int j = y; j < y + h; j++)
		for(int i = x; i < x + w; i++) {
			Layer& l = layers[operating_layer];
			if(l.cells.size() -1 < j*width+i)
				return;
			l.cells[j*width+i].bg = this->bg;
			l.cells[j*width+i].entries.clear();
			

		}
	}

	void Terminal::crop(int x, int y, int w, int h) {
		spdlog::warn("Unimplemented function: Terminal::crop");
	}

	void Terminal::construct_alias() {
		if(tileset == NULL)
			return;
		if(tileset->codes.size() == 0) 
			return;
		SDL_RendererInfo info;
		SDL_GetRendererInfo(renderer, &info);
		int maxX = info.max_texture_width / tileset->width();
		int texW, texH;
		int rows = ceil(tileset->codes.size() / maxX);
		if(rows < 1) rows = 1;
		if(rows == 1)
			texW = tileset->codes.size() * tileset->width();
		else
			texW = info.max_texture_width;
		texH = rows * tileset->height();
		spdlog::info("Tex W: {}, Tex H: {}", texW, texH);
		bool size_change = false;
		int w, h;
		SDL_QueryTexture(alias, NULL, NULL, &w, &h);
		if(w != texW || h != texH)
			size_change = false;
		if(alias != NULL && size_change) {
			SDL_DestroyTexture(alias);
			alias = NULL;
		}
		if(alias == NULL) {
			alias = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, texW, texH);
		}
		if(!alias) {
			spdlog::error("Something went wrong: {}", SDL_GetError());
			throw std::runtime_error("OOPS");
		}
		int x = 0;
		int y = 0;
		int idx = 1;
		std::vector<uint32_t> white;
		white.resize(tileset->width() * tileset->height(), 0xFFFFFFFF);
		clips.clear();
		{
			SDL_Rect r = { x* tileset->width(), y * tileset->height(), tileset->width(), tileset->height() };
			clips.push_back(r);
			SDL_UpdateTexture(alias, &r, &white[0], tileset->width() * 4);
			x++;
		}
		for(auto& gl : tileset->codes) {
			SDL_Rect r = { x* tileset->width(), y * tileset->height(), tileset->width(), tileset->height() };
			clips.push_back(r);
			SDL_UpdateTexture(alias, &r, &gl[0], tileset->width() * 4);
			if(idx == maxX) {
				y++;
				x = 0;
			}
			x++;
			idx++;
		}
		need_alias = false;
		SDL_SetTextureBlendMode(alias, SDL_BLENDMODE_BLEND);
	}
	
	inline int Terminal::render_entry(int x, int y, cell_entry& ent) {
		if(ent.glyph == 0)
			return -1;
		SDL_SetTextureColorMod(alias, (ent.fg >> 24) & 0xFF,
			(ent.fg >> 16) & 0xFF,
			(ent.fg >> 8 ) & 0xFF);
		SDL_SetTextureAlphaMod(alias, ent.fg & 0xFF);
		SDL_Rect source = clips[ent.glyph + 1];
		SDL_Rect destination = { x * tileset->width(), y * tileset->height(), tileset->width(), tileset->height() };
		if(ent.dx == 0 && ent.dy == 0) {
			SDL_RenderCopy(renderer, alias, &source, &destination);
			return 0;
		}
		if(ent.dx < 0) {
			int absdx = abs(ent.dx);
			source.x += absdx;
			source.w -= absdx;
			destination.w -= absdx;
		} else {
			source.w -= ent.dx;
			destination.x += ent.dx;
			destination.w -= ent.dx;
		}
		if(ent.dy < 0) {
			int absdy = abs(ent.dy);
			source.y += absdy;
			source.h -= absdy;
			destination.h -= absdy;
		} else {
			source.h -= ent.dy;
			destination.y += ent.dy;
			destination.h -= ent.dy;
		}
		SDL_RenderCopy(renderer, alias, &source, &destination);
		return 0;
	}

	void Terminal::refresh() {
		if(need_alias || alias == NULL) {
			construct_alias();
		}
		SDL_RenderClear(renderer);
		SDL_Rect bg = clips[0];
		int layer = 0;
		{
			int idx = 0;
			for(auto&cel : layers[0].cells) {
				int x = idx % width;
				int y = idx / width;
				SDL_SetTextureColorMod(alias, (cel.bg >> 24) & 0xFF,
					(cel.bg >> 16) & 0xFF,
					(cel.bg >> 8 ) & 0xFF);
				SDL_SetTextureAlphaMod(alias, cel.bg & 0xFF);
				SDL_Rect dest = { x * tileset->width(), y * tileset->height(), tileset->width(), tileset->height()};
				SDL_RenderCopy(renderer, alias, &bg, &dest);
				idx++;
			}
		}
		for(Layer& l : layers) {
			int idx = 0;
			for(auto &cel : l.cells) {
				int x = idx % width;
				int y = idx / width;
				if(layer == 0) {
				}
				if(comp) {
					for(auto &ent : cel.entries) {
						if(render_entry(x, y, ent) < 0)
							break;	
					}
				} else {
					if(cel.entries.size() > 0) {
						auto& ent = cel.entries[0];
						render_entry(x, y, ent);
					}
				}
				idx++;
			}
		}
		layer++;
		SDL_RenderPresent(renderer);
	}

	void Terminal::put(int x, int y, int code) {
		put_ex(x, y, 0, 0, code);
	}

	void Terminal::put_ex(int x, int y, int dx, int dy, int code) {
		cell& cel = layers[operating_layer].cells[y * width + x];
		if(operating_layer == 0)
			cel.bg = this->bg;
		if(comp) {
			cel.entries.emplace_back(dx, dy, code, fg);
		}
		else {
			cel.entries.resize(1);
			cel.entries[0] = {dx, dy, code, fg};	
		}
		layers[operating_layer].cells[y* width +x] = cel;
	}

	int Terminal::pick(int x, int y, int idx) {
		cell& cel = layers[operating_layer].cells[y * width + x];
		if(cel.entries.size() - 1 < idx)
			return 0;
		return cel.entries[idx].glyph;
	}

	uint32_t Terminal::pick_colour(int x, int y, int idx) {
		cell& cel = layers[operating_layer].cells[y * width + x];
		if(cel.entries.size() - 1 < idx)
			return 0;
		return cel.entries[idx].fg;
	}

	uint32_t Terminal::pick_bkcolour(int x, int y) {
		cell& cel = layers[operating_layer].cells[y * width + x];
		return cel.bg;	
	}

	int Terminal::state(int slot) {
		switch(slot) {
			case TERM_LAYER:
				return operating_layer;
			case TERM_COMPOSITION:
				return comp;
			default:
				return 0;
		}
	}

// ################# FUNCTIONS ############## //

	static Terminal* g_terminal = new Terminal();
	
	int open() {
		return g_terminal->open();
	}
	
	int close() {
		return g_terminal->close();
	}
	void size(int width, int height) {
		g_terminal->size(width, height);
	}
	void set_tileset(std::shared_ptr<Tileset> tileset) {
		g_terminal->set_tileset(tileset);
	}
	void set_fg(uint32_t fg) {
		g_terminal->set_fg(fg);
	}
	void set_bg(uint32_t bg) {
		g_terminal->set_bg(bg);
	}
	void composition(bool comp) {
		g_terminal->composition(comp);
	}
	void layer(int layer) {
		g_terminal->layer(layer);
	}

	void clear() {
		g_terminal->clear();
	}
	void clear_area(int x, int y, int w, int h) {
		g_terminal->clear_area(x, y, w, h);
	}
	void crop(int x, int y, int w, int h) {
		g_terminal->crop(x, y, w, h);
	}

	void refresh() {
		g_terminal->refresh();
	}
	
	void put(int x, int y, int code) {
		g_terminal->put(x,y,code);
	}
	void put_ex(int x, int y, int dx, int dy, int code) {
		g_terminal->put_ex(x,y,dx,dy,code);
	}
	int pick(int x, int y, int idx) {
		return g_terminal->pick(x, y, idx);
	}
	uint32_t pick_colour(int x, int y, int idx) {
		return g_terminal->pick_colour(x,y,idx);
	}
	uint32_t pick_bkcolour(int x, int y) {
		return g_terminal->pick_bkcolour(x,y);
	}
	int state(int slot) {
		return g_terminal->state(slot);
	}

	int has_input();
	int read();
	int peek();


}


