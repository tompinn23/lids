#include "Mod.hxx"

#include "../log.h"

#include "json.hpp"
#include "physfs.h"

namespace LuaApi {
    Mod::Mod(const std::string id, const std::string name, const std::string version, const std::string desc, const std::string author)
    	: _name(name), _version(version), _desc(desc), _author(author), _id(id) {

    }

	Mod Mod::from_json(const std::string modid) {
		auto fp = PHYSFS_openRead((modid + "/info.json").c_str());
		if(fp == NULL) {
			log_error("Missing info.json for mod: %s", modid);
			throw std::runtime_error("Failed to load json");
		}
		size_t sz = PHYSFS_fileLength(fp);
		char* arr = new char[sz + 1];
		PHYSFS_readBytes(fp, arr, sz);
		arr[sz] = '\0';
		PHYSFS_close(fp);
		auto json = nlohmann::json::parse(arr);
		return Mod(modid, json["name"], json["version"], json["desc"], json["author"]);	
	}
}
