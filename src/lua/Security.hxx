#pragma once

#include "sol/state_view.hpp"
#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "Mod.hxx"

#include <unordered_map>

namespace LuaApi {
	class Security {
	public:
		Security(sol::state_view lua, const Mod& ref);
		Security(sol::state_view lua, const Mod &&) = delete;
		int Require(sol::state_view lua, const std::string path);
	private:
		sol::environment sandbox;
		const Mod& ref;
	};

	class SecurityManager {
	public:
		static SecurityManager& Instance() {
			static SecurityManager instance;
			
			return instance;
		}
		SecurityManager(SecurityManager const&) = delete;
		void operator=(SecurityManager const&) = delete;
		Security& GetSecurityObj(std::string key);
		void SetSecurityObj(std::string key, Security obj); 
	private:
		std::unordered_map<std::string, Security> security_map;
		SecurityManager() {}

	};

};



