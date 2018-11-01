#include "MenuManager.h"

using namespace DirectX;

MenuManager::MenuManager(DirectX::SpriteFont * _font)
{
	font = _font;
	
	// initialize buttons
	startButton.text = L"Start";
	XMStoreFloat2(&startButton.size, font->MeasureString(startButton.text));
	startButton.pos = XMFLOAT2(100, 200);
	XMStoreFloat2(&startButton.origin, font->MeasureString(startButton.text) / 2.f);
}

MenuManager::~MenuManager()
{
}

void MenuManager::DisplayMainMenu(DirectX::SpriteBatch * spriteBatch)
{
	spriteBatch->Begin();

	font->DrawString(spriteBatch, startButton.text, startButton.pos, Colors::White, 0.f, startButton.origin);

	spriteBatch->End();
}

void MenuManager::ClickButton(float xPos, float yPos)
{
	XMFLOAT2 mousePos = XMFLOAT2(xPos, yPos);
}
