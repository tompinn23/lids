#include "console.hxx"
#include "fmt/printf.h"
#include "terminal.hxx"

#include "log.h"

const int thin_tl = 218;
const int thin_bl = 192;
const int thin_tr = 191;
const int thin_br = 217;
const int thin_h = 196;
const int thin_v = 179;

const int thick_h = 205;
const int thick_v = 186;
const int thick_tl = 201;
const int thick_bl = 200;
const int thick_br = 188;
const int thick_tr = 187;


Console::Console(int width, int height) :
	width(width), height(height) {
		term_layer.cells.resize(width * height);
}

void Console::put(int x, int y, int code) {
	put_ex(x, y, 0, 0, code);
}

void Console::put_ex(int x, int y, int dx, int dy, int code) {
	if(y < 0 || x < 0 || x >= width || y >= height) return;
	terminal::cell& cel = term_layer.cells[y * width + x];
	cel.bg = this->bg;
	if(terminal::state(terminal::TERM_COMPOSITION)) {
		cel.entries.emplace_back(dx, dy, code, fg);
	} else {
		cel.entries.resize(1);
		cel.entries[0] = {dx, dy, code, fg};
	}
}

void Console::clear(int x, int y) {
	term_layer.cells[y * width + x].entries.clear();
}

void Console::clear() {
	term_layer.clear(this->bg);
}

void Console::box(bool thick) {
	for(int i = 1; i < width - 1; i++) {
		put(i, 0, thick ? thick_h : thin_h);
		put(i, height - 1, thick ? thick_h : thin_h);
	}
	for(int i = 1; i < height - 1; i++) {
		put(0, i, thick ? thick_v : thin_v);
		put(width - 1, i, thick ? thick_v : thin_v);
	}
	put(0, 0, thick ? thick_tl : thin_tl);
	put(width - 1, 0, thick ? thick_tr : thin_tr);
	put(0, height - 1, thick ? thick_bl : thin_bl);
	put(width - 1, height - 1, thick ? thick_br : thin_br);
}

void Console::draw(int x, int y) {
	for(int j = 0; j < height; j++)
	for(int i = 0; i < width; i++) {
		terminal::raw_put(x + i, y + j, term_layer.cells[j * width + i]);
	}
}

void Console::con_printf(int x, int y, int sz_limit, const char* format, fmt::printf_args args) {
	std::string str = fmt::vsprintf(format, args);
	int x_off = 0;
	int max_c = str.size();
	if(sz_limit * 2 <= str.size() && sz_limit != 0) {
		max_c = sz_limit * 2;
		log_info("Using max_c = %d", max_c); 
	}
	for(int i = 0; i < max_c; i += 2) {
		clear(x + x_off, y);
		put(x + x_off, y, 256 + str[i]);
		put_ex(x + x_off, y, 6, 0, 256 + str[i+1]);
		x_off++;
	}
}

int Console::con_sizef(const char* format, fmt::printf_args args) {
	return ceil(fmt::vsprintf(format, args).size() / 2);
}
