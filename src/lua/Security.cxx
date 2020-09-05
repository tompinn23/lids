#include "Security.hxx"



#include <string>
#include "physfs.h"
#include "../log.h"


#include "Mod.hxx"
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
		log_error("got here");
		sol::state_view lua(L);
		std::string path = sol::stack::get<std::string>(L, 1);
		lua_Debug info;
		// Level 0 means current function (this C function, which is useless for our purposes)
		// Level 1 means next call frame up the stack. This is probably the environment we're looking for?
		int level = 2;
		int pre_stack_size = lua_gettop(L);
		if (lua_getstack(L, level, &info) != 1) {
			// failure: call it quits
			log_error("error: unable to traverse the stack");
			lua_settop(L, pre_stack_size);
			return 0;
		}
		// the "f" identifier is the most important here
		// it pushes the function running at `level` onto the stack:
		// we can get the environment from this
		// the rest is for printing / debugging purposes
		if (lua_getinfo(L, "fnluS", &info) == 0) {
			// failure?
			log_error("manually -- error: unable to get stack information");
			lua_settop(L, pre_stack_size);
			return 0;
		}

		// Okay, so all the calls worked.
		// Print out some information about this "level"
		std::cout << "manually -- [" << level << "] " << info.short_src << ":" << info.currentline
			<< " -- " << (info.name ? info.name : "<unknown>") << "[" << info.what << "]" << std::endl;
	
		// Grab the function off the top of the stack
		// remember: -1 means top, -2 means 1 below the top, and so on...
		// 1 means the very bottom of the stack, 
		// 2 means 1 more up, and so on to the top value...
		sol::function f(L, -1);
		// The environment can now be ripped out of the function
		sol::environment env(sol::env_key, f);
		if (!env.valid()) {
			log_error("manually -- error: no environment to get");
			lua_settop(L, pre_stack_size);
			return 0;
		}
		for(auto &a : env) {
			log_info("env: %s, %s", a.first.as<std::string>(), a.second.as<std::string>());
		}
		return SecurityManager::Instance().GetSecurityObj(env["key_fuck_key"])->Require(lua, path);
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
		
	}

	Security::~Security() {	
	}

	void Security::read_only_env(sol::table t, sol::stack_object val) {
		throw std::runtime_error("lua tried to change its environment.");	
	}

	void Security::SetSecurityKey(std::string uuid) {
		sandbox["key_fuck_key"] = uuid;
	}

	void Security::run(sol::state_view l, std::string file) {
		log_info("running script: %s", file);
		auto pfr = l.safe_script_file(file, ro_env, sol::script_pass_on_error);
		if(!pfr.valid()) {
			sol::error err = pfr;
			sol::call_status status = pfr.status();
			log_error("Lua Error :: type :: %s :: %s", sol::to_string(status), err.what());
		}
	}

	int Security::Require(sol::state_view lua, const std::string path) {
//      std::string name = ("mods/" + ModLoader::get_current()->name());
		auto name = ref.name();
		log_info("require working! %s", name);
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
		return 0;
	}


	std::shared_ptr<Security> SecurityManager::GetSecurityObj(std::string key) {
		return security_map.at(key);
	}

	std::string SecurityManager::AddSecurityObj(std::shared_ptr<Security> obj) {
		sole::uuid key = sole::uuid4();
		security_map.emplace(key.str(), obj);
		obj->SetSecurityKey(key.str());
		return key.str();
	}

	void SecurityManager::SetSecurityObj(std::string key, std::shared_ptr<Security> obj)	{
		security_map.emplace(key, obj);
	}

}
