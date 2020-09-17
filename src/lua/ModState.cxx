#include "ModState.hxx"

#include "../log.h"

namespace LuaApi {
	ModState::ModState(Mod& ref)
		: modInfo(ref) {
		lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table);
		lua.add_package_loader(LuaApi::LoadFileRequire, true);
		sec = new Security(lua, ref);
		lua.registry()["security"] = sec;
		sec->set_default_env(lua);
	}

	ModState::~ModState() {
		delete sec;
	}

	int	ModState::preInit() {
		log_info("Pre Init :: %s", modInfo.name());
		return sec->run(lua, "definitions.lua");
	}
}
