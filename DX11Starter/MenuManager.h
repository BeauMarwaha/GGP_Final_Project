#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <string>
#include <iostream>

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
	void DisplayMainMenu(DirectX::SpriteBatch * spriteBatch, ID3D11DeviceContext* context);
	void DisplayGameHUD(DirectX::SpriteBatch * spriteBatch, ID3D11DeviceContext* context, int asteroidCount);
	void DisplayGameOverMenu(DirectX::SpriteBatch * spriteBatch, ID3D11DeviceContext* context);
	bool DetectStartClick(int xPos, int yPos);
	bool DetectQuitClick(int xPos, int yPos);
};

