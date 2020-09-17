#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "Security.hxx"

namespace LuaApi {

class ModState {
public:
	ModState(Mod& ref);
	~ModState();
	int preInit();
	int Init();
	int postInit();
private:
	Mod& modInfo;
	Security* sec;
	sol::state lua;
};

}

