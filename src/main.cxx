
#include "console.hxx"
#include "terminal.hxx"

#include <SDL.h>
#include <physfs.h>

#include "lua/Mod.hxx"
#include "lua/ModState.hxx"
#include "Input.hxx"
#include "ui.hxx"

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
	PHYSFS_mount("data/test_mod", "test_mod", 1);
	using namespace LuaApi;
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
	lua.add_package_loader(LuaApi::LoadFileRequire, true);
	auto mod_info = Mod::from_json("test_mod");
	auto state = ModState(mod_info);
	state.preInit();
}

int main(int argc, char** argv) {
	log_info("Starting lids");
	PHYSFS_init(argv[0]);
	lua_testing();
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
	InputManager manager;
	init_commands(manager);
	manager.setCommmandDomain("ascii");
	bool exit = false;
	const uint32_t dt = 20;
	uint32_t currentTime = SDL_GetTicks();
	uint32_t accumulator = 0;	
	std::string fps;
	std::string txt = "";
	UITextBox txtbox = UITextBox(false, 14, 4);
	txtbox.SetText("Hello, my name is jimbo.!!");
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
		print(3, 3, txt);
		txtbox.Draw(6, 6);
		terminal::refresh();
		manager.Poll();
		Command cmd;
		while(manager.getEvent(&cmd)) {
			if(cmd.type == CMD_QUIT) 
				exit = true;
			txtbox.Update(cmd);
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
