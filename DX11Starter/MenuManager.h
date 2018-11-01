#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>

struct Button
{
	const wchar_t* text;
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT2 size;
	DirectX::XMFLOAT2 origin;
};

class MenuManager
{
private:
	DirectX::SpriteFont * font;

	Button startButton;
	Button quitButton;
public:
	
	MenuManager(DirectX::SpriteFont * _font);
	~MenuManager();
	void DisplayMainMenu(DirectX::SpriteBatch * spriteBatch);
	void ClickButton(float xPos, float yPos);
};

