cmake_minimum_required(VERSION 3.15)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(lyds)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_SKIP_BUILD_RPATH FALSE)


add_subdirectory(dependencies/SDL2-2.0.12)
add_subdirectory(dependencies/fmt-7.0.3)

add_subdirectory(dependencies/lua-5.4.0)
add_subdirectory(dependencies/sol2-3.2.1)
add_subdirectory(dependencies/physfs-3.0.2)
#target_compile_options(physfs-static PRIVATE -Wno-volatile)
add_library(json INTERFACE)
target_include_directories(json INTERFACE dependencies/json-3.9.1)
add_library(lodepng STATIC dependencies/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC dependencies/lodepng)

#add_library(sol INTERFACE)
#target_include_directories(sol INTERFACE dependencies/sol/include)


add_executable(lyds src/main.cxx 
	src/tileset.cxx
    src/terminal.cxx 
	src/console.cxx
	src/log.cxx
    src/lua/Security.cxx
    src/lua/Mod.cxx
    src/lua/ModLoader.cxx)
target_link_libraries(lyds SDL2 sol2 lua_static physfs-static json lodepng fmt::fmt)
if(MSVC)
target_link_libraries(lyds SDL2main)
endif()
target_include_directories(lyds PUBLIC dependencies/physfs-3.0.2/src dependencies/lua-5.4.0/src)
set_target_properties(lyds PROPERTIES BUILD_RPATH ${CMAKE_BINARY_DIR})

add_custom_command(
        TARGET lyds POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/Alloy.png
                ${CMAKE_CURRENT_BINARY_DIR}/Alloy.png)
                
add_custom_command(
        TARGET lyds POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/font.png
                ${CMAKE_CURRENT_BINARY_DIR}/font.png)
