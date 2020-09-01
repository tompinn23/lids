#include "console.hxx"
#include "terminal.hxx"

const int thin_tl = 208;
const int thin_bl = 183;
const int thin_tr = 182;
const int thin_br = 207;
const int thin_h = 187;

const int thick_h = 196;
const int thick_tl = 192;
const int thick_bl = 191;


Console::Console(int x, int y, int width, int height) :
	x(x), y(y), width(width), height(height) {
		term_layer.cells.resize(width * height);
}

void Console::put(int x, int y, int code) {
	put_ex(x, y, 0, 0, code);
}

void Console::put_ex(int x, int y, int dx, int dy, int code) {
	terminal::cell& cel = term_layer.cells[y * width + x];
	cel.bg = this->bg;
	if(terminal::state(terminal::TERM_COMPOSITION)) {
		cel.entries.emplace_back(dx, dy, code, fg);
	} else {
		cel.entries.resize(1);
		cel.entries[0] = {dx, dy, code, fg};
	}
}

void Console::box(int thick) {
	for(int i = 0; i < width; i++) {

	}
}
