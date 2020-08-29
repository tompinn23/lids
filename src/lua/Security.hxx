#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace LuaApi {
	class Security {
	public:
		Security(sol::state_view lua);
		int Require(sol::state_view lua, const std::string path);
	private:
		sol::environment sandbox;
	};
};
