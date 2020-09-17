#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "Mod.hxx"

#include <unordered_map>

namespace LuaApi {
	int lua_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description);
	int LoadFileRequire(lua_State* L);
	class Security {
	public:
		Security(sol::state_view lua, Mod& ref);
		Security(sol::state_view lua, Mod &&) = delete;
		~Security();
		void SetSecurityKey(std::string uuid);
		int Require(sol::state_view lua, const std::string path);
		static void read_only_env(sol::table t, sol::stack_object val);
		int run(sol::state_view l, std::string file);
		void set_default_env(sol::state_view lua);
		static void print(sol::this_state st, sol::variadic_args va);
	private:
		sol::table sandbox;
		sol::environment ro_env;
		Mod& ref;
	};

};



