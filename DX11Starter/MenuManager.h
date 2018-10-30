#pragma once
#include <SpriteFont.h>

enum class MenuState {
	Main = 1,
	Game = 2,
	GameOver = 3
};

class MenuManager
{
private:
	DirectX::SpriteFont * font;
public:
	MenuManager(DirectX::SpriteFont * _font);
	~MenuManager();
};

