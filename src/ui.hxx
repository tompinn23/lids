#pragma once

#include "Input.hxx"
#include "console.hxx"

enum {
	UI_STAT_EXIT,
	UI_STAT_CHANGE,
	UI_STAT_NOP
};


class UIWidget {
public:
	virtual int Update(Command& cmd) = 0;
	virtual void Draw(int x, int y) = 0;
protected:
	UIWidget(int w, int h);
	Console con;
};

class UITextBox : public UIWidget {
public:
	UITextBox(bool thick_border, int width, int height);
	void SetText(std::string text);
	int Update(Command& cmd);
	void Draw(int x, int y);
private:
	bool thick;
	std::string text;
	int lineLength;
	int lines;
};
