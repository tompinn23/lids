#include "GameScript.hxx"

namespace LuaApi {
	void GameScript::onTick(sol::protected_function hook) {
		onTickHooks.push_back(hook);
	}
	void GameScript::onInput(sol::protected_function hook) {
		onInputHooks.push_back(hook);
	}
}

