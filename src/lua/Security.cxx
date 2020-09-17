#include "Security.hxx"



#include <string>
#include "physfs.h"
#include "../log.h"


#include "Mod.hxx"
#include "sol/state_handling.hpp"
#include "sole.hpp"

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

		"require",
	};
	const std::vector<std::string> safe_libraries = {
		"string", "table", "math" 
	};

	int LoadFileRequire(lua_State* L) {
		sol::state_view lua(L);
		Security* sec = lua.registry()["security"];
		std::string path = sol::stack::get<std::string>(L, 1);
		return sec->Require(lua, path);
	}

	Security::Security(sol::state_view lua, Mod& ref) : ref(ref) {
		sandbox = sol::environment(lua, sol::create);
		ro_env = sol::environment(lua, sol::create);
		log_info("creating environment");
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
		}
		sandbox["string"]["rep"] = nullptr;
		auto mt = ro_env[sol::metatable_key];
		if (!mt.valid()) {
			mt = lua.create_table();
		}
		mt["__index"] = sandbox;
		mt["__newindex"] = Security::read_only_env;
		sandbox["print"] = Security::print;
	}

	Security::~Security() {	
	}
	
	void Security::print(sol::this_state st, sol::variadic_args va) {
		lua_State* L = st;
		lua_Debug info;
		int level = 1;
		int pre_stack_size = lua_gettop(L);
		bool failure = false;
		if (lua_getstack(L, level, &info) != 1) {
			// failure: call it quits
			failure = true;
			lua_settop(L, pre_stack_size);
		}
		// the "f" identifier is the most important here
		// it pushes the function running at `level` onto the stack:
		// we can get the environment from this
		// the rest is for printing / debugging purposes
		if (lua_getinfo(L, "lS", &info) == 0) {
			// failure?
			failure = true;
			lua_settop(L, pre_stack_size);
		}
		std::string str = "";
		for(auto arg : va) {
			if(arg.is<sol::nil_t>())
				str += "nil";
			else if(arg.is<bool>())
				str += (arg.as<bool>() ? "true" : "false");
			else if(arg.is<std::string>())
				str += arg;
			else if(arg.is<int>())
				str += fmt::sprintf("%d", arg.as<int>());
			else
				str += fmt::sprintf("%p",arg.as<sol::object>().pointer());
			str+= " ";
		}
		if(failure)
			fmt_log_log(LOG_INFO, "??lua", 0, "%s", str);
		else
			fmt_log_log(LOG_INFO, info.short_src, info.currentline, "%s", str);
	}

	void Security::read_only_env(sol::table t, sol::stack_object val) {
		throw std::runtime_error("lua tried to change its environment.");	
	}

	void Security::SetSecurityKey(std::string uuid) {
		sandbox["key_fuck_key"] = uuid;
	}

	int Security::run(sol::state_view l, std::string file) {
		log_info("running script: %s", file);
		auto fp = PHYSFS_openRead((ref.id() + "/" + file).c_str());
		if(fp == NULL) {
			log_error("Failed to load file %s from mod: %s", file, ref.id());
			return -1;
		}
		size_t sz = PHYSFS_fileLength(fp);
		char* arr = new char[sz + 1];
		PHYSFS_readBytes(fp, arr, sz);
		arr[sz] = '\0';
		PHYSFS_close(fp);
		auto pfr = l.safe_script(arr, ro_env, (ref.id() + "::" + file), sol::load_mode::text);
		if(!pfr.valid()) {
			sol::error err = pfr;
			sol::call_status status = pfr.status();
			log_error("Lua Error :: type :: %s :: %s", sol::to_string(status), err.what());
			return -1;
		}
		return 0;
	}

	int Security::Require(sol::state_view lua, const std::string path) {
//      std::string name = ("mods/" + ModLoader::get_current()->name());
		auto name = ref.id();
		if(PHYSFS_exists(name.c_str()) == 0) {
			sol::stack::push(lua.lua_state(), "Failed to find mod data. WTF!");
			return 1;
		}
		std::string converted = path;
		size_t pos = converted.find(".");
		while(pos != std::string::npos) {
			converted.replace(pos, 1, "/");
			pos = converted.find(".");
		}
		std::string full = name + "/" + converted + ".lua";
		log_info("requiring :: %s", full);
		auto fp = PHYSFS_openRead(full.c_str());
		if(fp == NULL) {
			sol::stack::push(lua.lua_state(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return 1;
		}
		size_t sz = PHYSFS_fileLength(fp);
		char* arr = new char[sz + 1];
		PHYSFS_readBytes(fp, arr, sz);
		arr[sz]='\0';
		luaL_loadbufferx(lua.lua_state(), arr, sz, full.c_str(), "t");
		delete[] arr;
		return 1;
	}
	
	void Security::set_default_env(sol::state_view lua) {
		// Get environment registry index
    	lua_rawgeti(lua, LUA_REGISTRYINDEX, ro_env.registry_index());
		// Set the global environment
		lua_rawseti(lua, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	}


}
