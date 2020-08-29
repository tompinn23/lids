#pragma once


#include <vector>
#include <cstdint>
#include <string>
#include <memory>


typedef void (*tileset_cb)(void* udata);

class Tileset {
public:
	Tileset() : m_width(0), m_height(0) { codes.resize(1); }
	Tileset(int width, int height) : m_width(width), m_height(height) {codes.resize(1); }
	void add_codepoint(int cp, std::vector<uint8_t> data);
	void rm_codepoint(int cp);
	void add_observer(tileset_cb cb, void* udata);
	void del_observer(tileset_cb cb);
	int width() { return m_width; }
	int height() { return m_height; }
	void combine(std::shared_ptr<Tileset> other, int start);
	static std::shared_ptr<Tileset> from_file(const std::string file, int width = -1, int height = -1);
	
	std::vector<std::vector<uint8_t>> codes;
private:
	void tileset_changed();
	int m_width;
	int m_height;
	std::vector<std::pair<tileset_cb, void*>> observers;
};
