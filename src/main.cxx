
#include "console.hxx"
#include "terminal.hxx"

#include <SDL.h>

#include "lua/Security.hxx"

#include "log.h"
#include "lodepng.h"

#include <memory>

void print(int x, int y, std::string str) {
	int x_off = 0;
	for(int i = 0; i < str.size(); i += 2) {
			terminal::put(x + x_off, y, 256 + str[i]);
			terminal::put_ex(x + x_off, y, 6, 0,256 + str[i + 1]);	
			x_off++;
			
	}
}

int main(int argc, char** argv) {
	log_info("Starting lids");
   	if(terminal::open() < 0)
		return -1;
	terminal::composition(true);
	auto tileset = Tileset::from_file("Alloy.png");
	if (tileset == NULL)
		return -1;
	log_info("W: %d H: %d", tileset->width(), tileset->height());
	{
		auto font = Tileset::from_file("font.png", 12, 12);
		tileset->combine(font, 256);
	}
	terminal::set_tileset(tileset);
	terminal::size(80, 50);
	bool exit = false;
	const uint32_t dt = 20;
	uint32_t currentTime = SDL_GetTicks();
	uint32_t accumulator = 0;	
	std::string fps;
	auto con1 = Console(30, 20);
	auto con2 = Console(20, 10);
	con1.box(true);
	con2.box(false);
	con2.printf(1, 1, "Hello Mun");
	con1.put(2, 3, 'A');
	con1.printf(2 , 5, "OMG PRINTF: %d", rand() % 256);
	while(!exit) {
		uint32_t newTime = SDL_GetTicks();
		uint32_t frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		terminal::set_bg(0x32AAb4FF);
		terminal::clear();
/*		for(int x = 0; x < 80; x++) 
		for(int y = 0; y < 50; y++) {
			terminal::set_fg(terminal::to_rgba(rand() % 255, rand() % 255, rand() % 255, rand() % 255));
			terminal::set_bg(terminal::to_rgba(rand() % 255, rand() % 255, rand() % 255, rand() % 255));
			terminal::put(x, y, rand() % 256);
		}*/
		con1.set_fg(terminal::to_rgba(rand() % 255, rand() % 255, rand() % 255, rand() % 255));
		con2.set_fg(terminal::to_rgba(rand() % 255, rand() % 255, rand() % 255, rand() % 255));
		con1.printf(2 , 4, "OMG PRINTF: %d", rand() % 256);
		con2.printf(3, 3, "WOAh: %d%d", rand() % 20, rand() % 1);
		terminal::layer(1);
		con1.draw(2, 2);
		con2.draw(10, 10);
		terminal::layer(0);
		for(int i = 0; i < 50; i++) {
			terminal::put(60, i, i + 200);
			print(61, i, std::to_string(i + 200));
		}
/*		for(int x = 0; x < 80; x++) {
			terminal::put(x, 1, x + 200);
			if(x > 0) {
				terminal::put_ex(x, 2, 6, 0, x);
				terminal::put_ex(x + 1, 2, -6, 0, x);
			}
		}
		for(int i = 0; i < 80; i++) {
			terminal::put(i, 4, 256 + i);
		}
		print(8, 8, "Hello World!"); */
		terminal::set_bg(0xab23fcff);
		terminal::set_fg(0xFFFFFFFF);
		if(frameTime != 0) {
			terminal::clear_area(0, 0, 10, 1);
			print(0, 0, fps);
		}
		terminal::refresh();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				exit = true;
				break;
			}
		}
		while(accumulator >= dt)
		{
			fps = fmt::format("Time: {:d}fps", 1000 / frameTime); 
			accumulator -= dt;
		}
	}
	/*	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
	auto sec = LuaApi::Security(lua);
	lua["security"] = sec;
	lua.script(R"(
		print(_ENV.security)
	)", sec.sandbox);
	*/
	return 0;
}
