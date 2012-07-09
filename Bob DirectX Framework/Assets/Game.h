
#pragma once

#include "D3DGraphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Sound.h"

class Game
{
public:
	Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer );
	void Go();
//	D3DGraphics gfx;
private:
	void ComposeFrame();
	/********************************/
	/*  User Functions              */

	/********************************/
private:
	D3DGraphics gfx;
	KeyboardClient kbd;
	MouseClient mouse;
	DSound audio;
	/********************************/
	/*  User Variables              */

	/********************************/
};