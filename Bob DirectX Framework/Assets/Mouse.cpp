
#include "Mouse.h"

MouseClient::MouseClient( const MouseServer& server )
: server( server )
{}
int MouseClient::GetMouseX() const
{
	return server.x;
}
int MouseClient::GetMouseY() const
{
	return server.y;
}
bool MouseClient::LeftIsPressed() const
{
	return server.leftIsPressed;
}
bool MouseClient::RightIsPressed() const
{
	return server.rightIsPressed;
}
bool MouseClient::IsInWindow() const
{
	return server.isInWindow;
}

MouseServer::MouseServer()
:	isInWindow( false ),
	leftIsPressed( false ),
	rightIsPressed( false ),
	x( -1 ),
	y( -1 )
{}
void MouseServer::OnMouseMove( int x,int y )
{
	this->x = x;
	this->y = y;
}
void MouseServer::OnMouseLeave()
{
	isInWindow = false;
}
void MouseServer::OnMouseEnter()
{
	isInWindow = true;
}
void MouseServer::OnLeftPressed()
{
	leftIsPressed = true;
}
void MouseServer::OnLeftReleased()
{
	leftIsPressed = false;
}
void MouseServer::OnRightPressed()
{
	rightIsPressed = true;
}
void MouseServer::OnRightReleased()
{
	rightIsPressed = false;
}
bool MouseServer::IsInWindow() const
{
	return isInWindow;
}