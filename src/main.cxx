
#include "console.hxx"
#include "terminal.hxx"

#include <SDL.h>
#include <physfs.h>

#include "lua/Security.hxx"
#include "lua/Mod.hxx"

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

void lua_testing() {
	using namespace LuaApi;
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
	lua.add_package_loader(LuaApi::LoadFileRequire, true);
	auto m = Mod("test", "1.0.0", "This is a test", "Tom Pinnock");
	auto sm = Security(lua, m);
	SecurityManager::Instance().AddSecurityObj(std::make_shared<Security>(sm));

	log_info("Running script");
	sm.run(lua, "test.lua");
	SecurityManager::Instance().clear_before_lua_dies();
}

int main(int argc, char** argv) {
	log_info("Starting lids");
	PHYSFS_init(argv[0]);
	lua_testing();
	return 0;

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
	auto sec = LuaApi::Security(lua);
	lua["security"] = sec;
	lua.script(R"(
		print(_ENV.security)
	)", sec.sandbox);
	*/
	return 0;
}
