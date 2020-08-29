#include "Mod.hxx"

namespace LuaApi {
    Mod::Mod(const std::string name, const std::string version, const std::string desc, const std::string author)
        : _name(name), _version(version), _desc(desc), _author(author) {

    }
}