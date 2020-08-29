#pragma once

#include "Mod.h"

#include <vector>
namespace LuaApi {
    class ModLoader {
        ModLoader();
        std::vector<Mod> GetModList();
    };
}