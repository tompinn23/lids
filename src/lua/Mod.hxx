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
        const std::string& name() { return _name; }
        const std::string& version() { return _version; }
        const std::string& desc() { return _desc; }
        const std::string& author() { return _author; }
    };
} // namespace LuaApi
