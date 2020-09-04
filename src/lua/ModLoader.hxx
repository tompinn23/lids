#pragma once

#include "Mod.hxx"

#include <vector>
namespace LuaApi {
    class ModLoader {
	public:
        ModLoader();
        std::vector<Mod> GetModList();

    };
}
