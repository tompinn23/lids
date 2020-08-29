#include "sdl2_render.hxx"

#include "drawing.hxx"

SDL_Texture* sdl2_renderer::get_alias_tex(int c) {
	int	idx;
	for(idx = 0; idx < alias_lim.size(); idx++) {
		if(c < alias_lim[idx])
			break;
	}
	if(idx < aliases.size()) {
		return aliases[idx];		
	}
	return 0;
}

SDL_Rect sdl2_renderer::get_clipping_box(int c) {
	return {0,0,0,0};
}
