#include "Input.hxx"
#include "SDL_events.h"

int InputManager::AddCommandSpec(std::string dom, CommandSpec spc) {
	commands[dom].insert({spc.keycode, spc});
	return 0;
}

bool InputManager::hasEvent() {
	return !commandQ.empty();
}

int InputManager::getEvent(Command* cmd) {
	if(!commandQ.empty()) {
		(*cmd) = std::move(commandQ.front());
		commandQ.pop();
		return 1;
	}
	return 0;
}

void InputManager::Poll() {
	SDL_Event evt;
	while(SDL_PollEvent(&evt)) {
		bool shift = false;
		bool ctrl = false;
		bool alt = false;
		bool caps = false;
		if(evt.type == SDL_QUIT) {
			Command cmd;
			cmd.type = CMD_QUIT;
			commandQ.push(cmd);
		}
		if(evt.type == SDL_KEYDOWN) {
			if(KMOD_CTRL & evt.key.keysym.mod)
				ctrl = true;
			if(KMOD_SHIFT & evt.key.keysym.mod)
				shift = true;
			if(KMOD_ALT & evt.key.keysym.mod)
				alt = true;
			if(KMOD_CAPS & evt.key.keysym.mod)
				caps = true;
			auto itPair = commands[dom].equal_range(evt.key.keysym.sym);
			for(auto it = itPair.first; it != itPair.second; ++it) {	
				auto spec = it->second;
				if(((spec.ctrl == ctrl && 
				   spec.alt == alt &&
				   spec.shft == shift &&
				   spec.caps == caps) || spec.ignore_mod) &&
				   spec.keycode == evt.key.keysym.sym) {
					Command cmd;
					if(spec.data_cb)
						spec.data_cb(spec.type, &cmd, spec.udata);
					cmd.type = spec.type;
					commandQ.push(cmd);
				}
			}	
		}
	}

}

void init_commands(InputManager& manager) {
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_E, "move_e", true, SDLK_RIGHT, true, false, false, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_N, "move_n", true, SDLK_UP, true, false, false, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_NE, "move_ne", true, SDLK_RIGHT, true, true, false, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_W, "move_w", true, SDLK_LEFT, true, false, false, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_NW, "move_nw", true, SDLK_LEFT, true, true, false, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_S, "move_s", true, SDLK_DOWN, true, false, false, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_SE, "move_se", true, SDLK_RIGHT, true, false, true, false, false));
	manager.AddCommandSpec("default", CommandSpec(CMD_MOVE_SW, "move_sw", true, SDLK_LEFT, true, false, true, false, false));

	manager.AddCommandSpec("ui", CommandSpec(CMD_MENU_DOWN, "menu_down", true, SDLK_DOWN, true, false, false, false, false));
	manager.AddCommandSpec("ui", CommandSpec(CMD_MENU_UP, "menu_up", true, SDLK_UP, true, false, false, false, false));
	manager.AddCommandSpec("ui", CommandSpec(CMD_MENU_ENTER, "menu_enter", true, SDLK_RETURN, true, false, false, false, false));

	manager.AddCommandSpec("ascii", CommandSpec(CMD_SPACE_K, "txt_spc", true, SDLK_SPACE, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_EXCL_K, "txt_!", true, SDLK_1, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_QUOTE_K, "txt_\"", true, SDLK_2, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_HASH_K, "txt_#", true, SDLK_3, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_DOLLAR_K, "txt_$", true, SDLK_4, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_PERCENT_K, "txt_%", true, SDLK_5, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_AMPERSAND_K, "txt_&", true, SDLK_7, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_APOSTROPHE_K, "txt_'", true, SDLK_QUOTE, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_LEFT_PAREN_K, "txt_(", true, SDLK_9, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_RIGHT_PAREN_K, "txt_)", true, SDLK_0, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_ASTERIX_K, "txt_*", true, SDLK_8, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_PLUS_K, "txt_+", true, SDLK_EQUALS, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_COMMA_K, "txt_,", true, SDLK_COMMA, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_MINUS_K, "txt_-", true, SDLK_MINUS, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_STOP_K, "txt_.", true, SDLK_PERIOD, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_SLASH_K, "txt_/", true, SDLK_SLASH, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_0_K, "txt_0", true, SDLK_0, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_1_K, "txt_1", true, SDLK_1, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_2_K, "txt_2", true, SDLK_2, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_3_K, "txt_3", true, SDLK_3, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_4_K, "txt_4", true, SDLK_4, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_5_K, "txt_5", true, SDLK_5, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_6_K, "txt_6", true, SDLK_6, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_7_K, "txt_7", true, SDLK_7, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_8_K, "txt_8", true, SDLK_8, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_9_K, "txt_9", true, SDLK_9, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_COLON_K, "txt_:", true, SDLK_SEMICOLON, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_SEMICOLON_K, "txt_;", true, SDLK_SEMICOLON, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_LT_K, "txt_<", true, SDLK_COMMA, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_EQUALS_K, "txt_=", true, SDLK_EQUALS, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_GT_K, "txt_>", true, SDLK_PERIOD, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_QUESTION_K, "txt_?", true, SDLK_SLASH, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_AT_K, "txt_@", true, SDLK_QUOTE, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_A_K, "txt_A", true, SDLK_a, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_A_K, "txt_A_cap", true, SDLK_a, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_a_K, "txt_a", true, SDLK_a, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_B_K, "txt_B", true, SDLK_b, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_B_K, "txt_B_cap", true, SDLK_b, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_b_K, "txt_b", true, SDLK_b, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_C_K, "txt_C", true, SDLK_c, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_C_K, "txt_C_cap", true, SDLK_c, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_c_K, "txt_c", true, SDLK_c, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_D_K, "txt_D", true, SDLK_d, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_D_K, "txt_D_cap", true, SDLK_d, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_d_K, "txt_d", true, SDLK_d, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_E_K, "txt_E", true, SDLK_e, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_E_K, "txt_E_cap", true, SDLK_e, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_e_K, "txt_e", true, SDLK_e, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_F_K, "txt_F", true, SDLK_f, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_F_K, "txt_F_cap", true, SDLK_f, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_f_K, "txt_f", true, SDLK_f, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_G_K, "txt_G", true, SDLK_g, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_G_K, "txt_G_cap", true, SDLK_g, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_g_K, "txt_g", true, SDLK_g, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_H_K, "txt_H", true, SDLK_h, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_H_K, "txt_H_cap", true, SDLK_h, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_h_K, "txt_h", true, SDLK_h, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_I_K, "txt_I", true, SDLK_i, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_I_K, "txt_I_cap", true, SDLK_i, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_i_K, "txt_i", true, SDLK_i, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_J_K, "txt_J", true, SDLK_j, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_J_K, "txt_J_cap", true, SDLK_j, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_j_K, "txt_j", true, SDLK_j, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_K_K, "txt_K", true, SDLK_k, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_K_K, "txt_K_cap", true, SDLK_k, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_k_K, "txt_k", true, SDLK_k, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_L_K, "txt_L", true, SDLK_l, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_L_K, "txt_L_cap", true, SDLK_l, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_l_K, "txt_l", true, SDLK_l, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_M_K, "txt_M", true, SDLK_m, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_M_K, "txt_M_cap", true, SDLK_m, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_m_K, "txt_m", true, SDLK_m, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_N_K, "txt_N", true, SDLK_n, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_N_K, "txt_N_cap", true, SDLK_n, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_n_K, "txt_n", true, SDLK_n, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_O_K, "txt_O", true, SDLK_o, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_O_K, "txt_O_cap", true, SDLK_o, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_o_K, "txt_o", true, SDLK_o, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_P_K, "txt_P", true, SDLK_p, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_P_K, "txt_P_cap", true, SDLK_p, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_p_K, "txt_p", true, SDLK_p, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_Q_K, "txt_Q", true, SDLK_q, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_Q_K, "txt_Q_cap", true, SDLK_q, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_q_K, "txt_q", true, SDLK_q, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_R_K, "txt_R", true, SDLK_r, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_R_K, "txt_R_cap", true, SDLK_r, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_r_K, "txt_r", true, SDLK_r, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_S_K, "txt_S", true, SDLK_s, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_S_K, "txt_S_cap", true, SDLK_s, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_s_K, "txt_s", true, SDLK_s, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_T_K, "txt_T", true, SDLK_t, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_T_K, "txt_T_cap", true, SDLK_t, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_t_K, "txt_t", true, SDLK_t, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_U_K, "txt_U", true, SDLK_u, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_U_K, "txt_U_cap", true, SDLK_u, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_u_K, "txt_u", true, SDLK_u, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_V_K, "txt_V", true, SDLK_v, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_V_K, "txt_V_cap", true, SDLK_v, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_v_K, "txt_v", true, SDLK_v, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_W_K, "txt_W", true, SDLK_w, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_W_K, "txt_W_cap", true, SDLK_w, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_w_K, "txt_w", true, SDLK_w, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_X_K, "txt_X", true, SDLK_x, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_X_K, "txt_X_cap", true, SDLK_x, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_x_K, "txt_x", true, SDLK_x, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_Y_K, "txt_Y", true, SDLK_y, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_Y_K, "txt_Y_cap", true, SDLK_y, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_y_K, "txt_y", true, SDLK_y, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_Z_K, "txt_Z", true, SDLK_z, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_Z_K, "txt_Z_cap", true, SDLK_z, true, false, false, false, true));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_z_K, "txt_z", true, SDLK_z, true, false, false, false, false));
	// IM NOT REFORMATTING AGAIN.
	manager.AddCommandSpec("ascii", CommandSpec(CMD_L_SQ_K, "txt_[", true, SDLK_LEFTBRACKET, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_B_SLASH_K, "txt_\\", true, SDLK_BACKSLASH, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_R_SQ_K, "txt_]", true, SDLK_RIGHTBRACKET, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_CARET_K, "txt_^", true, SDLK_6, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_UNDERSCORE_K, "txt__", true, SDLK_MINUS, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_GRAVE_K, "txt_`", true, SDLK_BACKQUOTE, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_L_BRACKET_K, "txt_{", true, SDLK_LEFTBRACKET, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_PIPE_K, "txt_|", true, SDLK_BACKSLASH, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_R_BRACKET_K, "txt_}", true, SDLK_RIGHTBRACKET, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_TILDA_K, "txt_~", true, SDLK_BACKQUOTE, true, true, false, false, false));

	//manager.AddCommandSpec("ascii", CommandSpec(CMD_BACKSPACE, "txt_BK", true, SDLK_BACKSPACE, true, false, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_BACKSPACE, SDLK_BACKSPACE, "txt_BK", false, false, false, false, false, true));
	//manager.AddCommandSpec("ascii", CommandSpec(CMD_ENTER, "txt_RT", true, SDLK_RETURN, true, true, false, false, false));
	manager.AddCommandSpec("ascii", CommandSpec(CMD_ENTER, SDLK_RETURN, "txt_RT£", false, false, false, false, false, true));

}

