#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "Mod.hxx"

#include <unordered_map>

namespace LuaApi {
	int LoadFileRequire(lua_State* L);
	class Security {
	public:
		Security(sol::state_view lua, Mod& ref);
		Security(sol::state_view lua, Mod &&) = delete;
		~Security();
		void SetSecurityKey(std::string uuid);
		int Require(sol::state_view lua, const std::string path);
		static void read_only_env(sol::table t, sol::stack_object val);
		void run(sol::state_view l, std::string file);
	private:
		sol::table sandbox;
		sol::environment ro_env;
		Mod& ref;
	};

	class SecurityManager {
	public:
		static SecurityManager& Instance() {
			static SecurityManager instance;
			
			return instance;
		}
		SecurityManager(SecurityManager const&) = delete;
		void operator=(SecurityManager const&) = delete;
		std::shared_ptr<Security> GetSecurityObj(std::string key);
		std::string AddSecurityObj(std::shared_ptr<Security> obj);
		void SetSecurityObj(std::string key, std::shared_ptr<Security> obj); 
		void clear_before_lua_dies() { security_map.clear(); }
	private:
		std::unordered_map<std::string, std::shared_ptr<Security>> security_map;
		SecurityManager() {}

	};

};



