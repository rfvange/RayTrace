#include "Game.h"
#include "bob_math.hpp"
#include "bob_graph.hpp"
#include "raytrace.hpp"
#include <vector>

Game::Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer )
{}

void Game::Go()
{
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}

//typedef int MyCode[4]; // for some reason doesn't work with arrays.
//using namespace std;

extern std::vector<MyCode> codeList;
extern Name OutputFile;

float bob_test = 3;
int bob_loop;
int ba, bb;
bool start_loop = true;

//Name myfile = "D:\\Bob3\\csrc\\advgr\\part4\\pyramid.rt";
Name myfile = "D:\\Bob3\\csrc\\advgr\\part4\\stack.rt"; // stack, molecule, planets

void Game::ComposeFrame()
{
	if( mouse.IsInWindow() )
	{
//		gfx.DrawLine(400, 300, mouse.GetMouseX(), mouse.GetMouseY(), 255, 255, 0);
//		gfx.PutPixel( bob_loop+25, bob_loop+50, 255, 255, 255 );
		if(bob_test < 200)
		{
			bob_loop = Round(bob_test);
			bob_test += 0.1f;
		}
//		TestPlot();
		InitPlotting(mouse.GetMouseX()/4,mouse.GetMouseY()/4);
		if(start_loop)
		{
			InitNoise();
			ClearMemory();
			GetData(myfile);
			start_loop = false;
		}
		Scan(OutputFile);
		for(int i=0; i<codeList.size(); i++)
		{
			int b = codeList.at(i).c;
//			b=256;
			b=b*15+253;
			int r = b&0xf00;
			r = r>>8;
			int g = b&0xf0;
			g = g>>4;
			b = b&0xf;
			int myx = codeList.at(i).a;
			if(myx >= 750) myx = 750;
			int myy = codeList.at(i).b;
			if(myy >= 650) myy = 650;
			gfx.PutPixel(myx*0.85,myy*0.85,(b*2/3 + g/3)*26,(g*2/3 + b/3)*16,(g/3 + b*2/3)*16);
		}
		codeList.clear();
//		DisplayAxis();
//		ba = codeList.size(); bb = codeList.at(0).b;
//		gfx.DrawDisc(100+ba/5, 100, 20, 255,0,255);
//		for(int i = 0; i<ba; i++)
//		{
//			gfx.PutPixel(2*codeList.at(i).a+50,2*codeList.at(i).b+50, 255,255,0);
//		}
//		gfx.DrawDisc( 200 + bob_loop,150 + bob_loop,20+bob_loop/5,255,255,0 );
//		codeList.clear();
	}
}

