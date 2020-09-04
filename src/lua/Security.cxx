#include "Security.hxx"

#include <string>
#include "physfs.h"

#include "Mod.hxx"

namespace LuaApi {
	
	const std::vector<std::string> whitelisted_builtins = {
		"assert",
		"error",
		"ipairs",
		"next",
		"pairs",
		"pcall",
		"print",
		"select",
		"tonumber",
		"tostring",
		"type",
		"unpack",
		"_VERSION",
		"xpcall",
	};
	const std::vector<std::string> safe_libraries = {
		"string", "table", "math" 
	};

	Security::Security(sol::state_view lua, const Mod& ref) : ref(ref) {
		sandbox = sol::environment(lua, sol::create);
		for(auto& func : whitelisted_builtins) {
			sandbox[func] = lua[func];
		}
		for(const std::string &name : safe_libraries) {
			sol::table copy(lua, sol::create);
			sol::table mod = lua[name];
			for(auto pair : mod) {
				copy[pair.first] = pair.second;
			}
			sandbox[name] = copy;
			sandbox["string"]["rep"] = nullptr;
		}
	}

	int Security::Require(sol::state_view lua, const std::string path) {
//      std::string name = ("mods/" + ModLoader::get_current()->name());
		std::string name = "FIXME";
		if(PHYSFS_exists(name.c_str()) == 0) {
			sol::stack::push(lua.lua_state(), "Failed to find file.");
			return 1;
		}
		std::string converted = path;
		size_t pos = converted.find(".");
		while(pos != std::string::npos) {
			converted.replace(pos, 1, "/");
			pos = converted.find(".");
		}
		std::string full = name + "/" + converted;
		PHYSFS_Stat st;		
		if(PHYSFS_stat(converted.c_str(), &st) == 0) {
			
		}
		return 1;
	}

	int LoadFileRequire(lua_State* L) {
		sol::state_view lua(L);
		LuaApi::SecurityManager
		std::string path = sol::stack::get<std::string>(L, 1);
		return sec.Require(lua, path);
	}

}
