#pragma once

#include <string>


namespace LuaApi
{
    class Mod {
    private:
        const std::string _name;
        const std::string _version;
        const std::string _desc;
        const std::string _author;
    public:
        Mod(const std::string name, const std::string version, const std::string desc, const std::string author);
        std::string name() { return _name; }
        std::string version() { return _version; }
        std::string desc() { return _desc; }
        std::string author() { return _author; }
    };
} // namespace LuaApi
