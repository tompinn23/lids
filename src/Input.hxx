#pragma once

#include <map>
#include <queue>
#include <string>
#include <stdint.h>
#include <SDL.h>
#include <unordered_map>

enum CommandType {
		CMD_RESERVE_K = 31,
		CMD_SPACE_K,
		CMD_EXCL_K,
		CMD_QUOTE_K,
		CMD_HASH_K,
		CMD_DOLLAR_K,
		CMD_PERCENT_K,
		CMD_AMPERSAND_K,
		CMD_APOSTROPHE_K,
		CMD_LEFT_PAREN_K,
		CMD_RIGHT_PAREN_K,
		CMD_ASTERIX_K,
		CMD_PLUS_K,
		CMD_COMMA_K,
		CMD_MINUS_K,
		CMD_STOP_K,
		CMD_SLASH_K,
		CMD_0_K,
		CMD_1_K,
		CMD_2_K,
		CMD_3_K,
		CMD_4_K,
		CMD_5_K,
		CMD_6_K,
		CMD_7_K,
		CMD_8_K,
		CMD_9_K,
		CMD_COLON_K,
		CMD_SEMICOLON_K,
		CMD_LT_K,
		CMD_EQUALS_K,
		CMD_GT_K,
		CMD_QUESTION_K,
		CMD_AT_K,
		CMD_A_K,
		CMD_B_K,
		CMD_C_K,
		CMD_D_K,
		CMD_E_K,
		CMD_F_K,
		CMD_G_K,
		CMD_H_K,
		CMD_I_K,
		CMD_J_K,
		CMD_K_K,
		CMD_L_K,
		CMD_M_K,
		CMD_N_K,
		CMD_O_K,
		CMD_P_K,
		CMD_Q_K,
		CMD_R_K,
		CMD_S_K,
		CMD_T_K,
		CMD_U_K,
		CMD_V_K,
		CMD_W_K,
		CMD_X_K,
		CMD_Y_K,
		CMD_Z_K,
		CMD_L_SQ_K,
		CMD_B_SLASH_K,
		CMD_R_SQ_K,
		CMD_CARET_K,
		CMD_UNDERSCORE_K,
		CMD_GRAVE_K,
		CMD_a_K,
		CMD_b_K,
		CMD_c_K,
		CMD_d_K,
		CMD_e_K,
		CMD_f_K,
		CMD_g_K,
		CMD_h_K,
		CMD_i_K,
		CMD_j_K,
		CMD_k_K,
		CMD_l_K,
		CMD_m_K,
		CMD_n_K,
		CMD_o_K,
		CMD_p_K,
		CMD_q_K,
		CMD_r_K,
		CMD_s_K,
		CMD_t_K,
		CMD_u_K,
		CMD_v_K,
		CMD_w_K,
		CMD_x_K,
		CMD_y_K,
		CMD_z_K,
		CMD_L_BRACKET_K,
		CMD_PIPE_K,
		CMD_R_BRACKET_K,
		CMD_TILDA_K = 126,
		CMD_BACKSPACE,
		CMD_ENTER,
		CMD_MOVE_N,
		CMD_MOVE_E,
		CMD_MOVE_NE,
		CMD_MOVE_W,
		CMD_MOVE_NW,
		CMD_MOVE_S,
		CMD_MOVE_SW,
		CMD_MOVE_SE,
		CMD_HIT,
		CMD_MENU_DOWN,
		CMD_MENU_UP,
		CMD_MENU_ENTER,

		CMD_QUIT,
};




struct Command;
typedef void (*command_fill_data_cb)(CommandType, Command*, void*);
struct Command {
	CommandType type;
	void* data;
};

struct CommandSpec {
	CommandSpec(CommandType ty, SDL_Keycode key, std::string name, bool shft, bool ctrl, bool alt, bool caps, bool iskb, bool ignore_mod)
		: type(ty), keycode(key), name(name), shft(shft), ctrl(ctrl), alt(alt), caps(caps), iskb(iskb), ignore_mod(ignore_mod), data_cb(NULL), udata(NULL) {}
	CommandSpec(CommandType ty, std::string name, bool iskb, SDL_Keycode key,bool pressed, bool shft, bool ctrl, bool alt, bool cpslk)
		: type(ty), keycode(key), name(name), shft(shft), ctrl(ctrl), alt(alt), iskb(iskb), caps(cpslk), ignore_mod(false), data_cb(NULL), udata(NULL) {}
	CommandSpec(CommandType ty, SDL_Keycode key, std::string name, bool shft, bool ctrl, bool alt, bool iskb, command_fill_data_cb cb, void* udata) 
		: type(ty), keycode(key), name(name), shft(shft), ctrl(ctrl), alt(alt), iskb(iskb), ignore_mod(false), data_cb(cb), udata(udata) {}
	CommandType type;	
	SDL_Keycode keycode;
	bool shft, ctrl, alt, iskb, caps, ignore_mod;
	std::string name;
	command_fill_data_cb data_cb = NULL;
	void* udata;
};

struct mouse_pos {
	uint32_t x;
	uint32_t y;
};

class InputManager {
public:
	mouse_pos getMousePos();	
	int AddCommandSpec(std::string dom, CommandSpec spc);
	void setCommmandDomain(std::string dom) { this->dom = dom; }
	std::string getCommandDomain() { return dom; }
	void Poll();
	bool hasEvent();
	
	int getEvent(Command* cmd);
private:
	std::queue<Command> commandQ;
	std::unordered_map<std::string, std::multimap<SDL_Keycode, CommandSpec>> commands;
	std::string dom;
};

void init_commands(InputManager& manager);
