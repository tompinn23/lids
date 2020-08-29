#include "tileset.hxx"

#include <lodepng.h>
#include <spdlog/spdlog.h>
void Tileset::add_codepoint(int cp, std::vector<uint8_t> data) {
	if(codes.size() - 1 < cp) {
		codes.resize(cp + 1);
	}
	codes[cp] = data;
	tileset_changed();
}

void Tileset::rm_codepoint(int cp) {
	codes[cp] = {};
	tileset_changed();
}

void Tileset::add_observer(tileset_cb cb, void* udata) {
	observers.emplace_back(cb, udata);
}

void Tileset::del_observer(tileset_cb cb) {
	for(int i = 0; i < observers.size(); i++) {
		if(observers[i].first == cb) {
			observers.erase(observers.begin() + i);
		}
	}
}

void Tileset::tileset_changed() {
	for(auto& call : observers) {
		call.first(call.second);
	}
}

void Tileset::combine(std::shared_ptr<Tileset> other, int start) {
	for(int idx = 0; idx < other->codes.size(); idx++) {
		add_codepoint(start + idx, std::move(other->codes[idx]));
	}
}
std::shared_ptr<Tileset> Tileset::from_file(const std::string file, int width, int height) {
	auto tileset = new Tileset();
	std::vector<unsigned char> out;
	unsigned w, h, error;
	error = lodepng::decode(out, w, h, file, LCT_RGBA);
	if(error) {
		spdlog::error("Error decoding png: {}", lodepng_error_text(error));
		return nullptr;
	}
	int actual_w = w / 16;
	int actual_h = h / 16;
	if(width == -1)
		tileset->m_width = actual_w;
	else
		tileset->m_width = width;
	if(height == -1)
		tileset->m_height = actual_h;
	else
		tileset->m_height = height;
	uint8_t r, g, b, a;
	bool color_key =false;
	a = out[3];
	if(a != 0) {
		r = out[0];
		g = out[1];
		b = out[2];
		color_key = true;
	}
	int idx = 0;
	for(int y = 0; y < 16; y++)
	for(int x = 0; x < 16; x++) {
		std::vector<uint8_t> img;
		img.resize(4 * tileset->height() * tileset->width(), 0);
		for(int i = 0; i < actual_h; i++) {
			int img_off = i * tileset->width() * 4;
			int pix_off = ((y * actual_h) * (w * 4) + (x * actual_w * 4)) + (i * (w * 4)); 
			memcpy(&img[img_off], &out[pix_off], actual_w * 4);
		}
		if(color_key) {
			for(int i = 0; i < img.size(); i+= 4) {
				if(img[i] == r && img[i+1] == g && img[i+2] == b) {
					img[i+3] = 0;
				}
			}
		}
		tileset->add_codepoint(idx, img);
		idx++;
	}
	return std::shared_ptr<Tileset>(tileset);
}
