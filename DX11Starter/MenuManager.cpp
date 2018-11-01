#include "MenuManager.h"

using namespace DirectX;

MenuManager::MenuManager(DirectX::SpriteFont * _font)
{
	font = _font;
	
	// initialize buttons
	startButton.text = L"Start";
	XMStoreFloat2(&startButton.size, font->MeasureString(startButton.text));
	startButton.pos = XMFLOAT2(600, 300);
	XMStoreFloat2(&startButton.origin, font->MeasureString(startButton.text) / 2.f);
	quitButton.text = L"Quit";
	XMStoreFloat2(&quitButton.size, font->MeasureString(quitButton.text));
	quitButton.pos = XMFLOAT2(600, 400);
	XMStoreFloat2(&quitButton.origin, font->MeasureString(quitButton.text) / 2.f);
}

MenuManager::~MenuManager()
{
}

void MenuManager::DisplayMainMenu(DirectX::SpriteBatch * spriteBatch)
{
	spriteBatch->Begin();
	const wchar_t* title = L"Asteroids";
	XMFLOAT2 titleOrigin;
	XMStoreFloat2(&titleOrigin, font->MeasureString(title) / 2.f);
	font->DrawString(spriteBatch, title, XMFLOAT2(600, 200), Colors::White, 0.f, titleOrigin);
	font->DrawString(spriteBatch, startButton.text, startButton.pos, Colors::White, 0.f, startButton.origin);
	font->DrawString(spriteBatch, quitButton.text, quitButton.pos, Colors::White, 0.f, quitButton.origin);

	spriteBatch->End();
}

bool MenuManager::DetectStartClick(float xPos, float yPos)
{
	return ((xPos > (startButton.pos.x - (startButton.size.x / 2.f))
		&& (xPos < startButton.pos.x + (startButton.size.x / 2.f)))
		&& ((yPos >(startButton.pos.y - (startButton.size.y / 2.f))
		&& (yPos < startButton.pos.y + (startButton.size.y / 2.f)))));
}

bool MenuManager::DetectQuitClick(float xPos, float yPos)
{
	return ((xPos >(quitButton.pos.x - (quitButton.size.x / 2.f))
		&& (xPos < quitButton.pos.x + (quitButton.size.x / 2.f)))
		&& ((yPos >(quitButton.pos.y - (quitButton.size.y / 2.f))
		&& (yPos < quitButton.pos.y + (quitButton.size.y / 2.f)))));
}
