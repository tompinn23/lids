#include "ui.hxx"

#include "log.h"

UIWidget::UIWidget(int w, int h) : con(w, h) {
}

UITextBox::UITextBox(bool thick_border, int width, int height) 
	: UIWidget(width, height), thick(thick_border), lineLength(width - 4), lines(height - 2) {
	con.box(thick);
}

void UITextBox::SetText(std::string text) {
	this->text = text;
	int idx = 0;
	for(int y = 1; y < lines + 1; y++) {
		con.printf(2, y, lineLength, "%s", text.substr(idx));
		idx += lineLength * 2;
		if(lineLength < text.size())
			continue;
		else
			break;
	}
}

int UITextBox::Update(Command& cmd) {
	switch(cmd.type) {
		case CMD_ENTER:
			return UI_STAT_EXIT;
		default:
			return UI_STAT_NOP;
	}
}

void UITextBox::Draw(int x, int y) {
	con.draw(x, y);
}
