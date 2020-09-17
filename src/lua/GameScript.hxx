#pragma once

#include <vector>

#include "sol/sol.hpp"

namespace LuaApi {
	class GameScript {
	public:
		void onTick(sol::protected_function hook);
		void onInput(sol::protected_function hook);
	private:
		std::vector<sol::protected_function> onTickHooks;
		std::vector<sol::protected_function> onInputHooks;
	};
};
