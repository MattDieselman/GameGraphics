#include "InputManager.h"
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

// ---------- KEYBOARD INPUT ---------------------------------------------------

void InputManager::update(Entity* player)
{
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		player->Move(0.015, XMFLOAT3(0, 1, 0));
	}
	else
	{
		player->Move(0.01, XMFLOAT3(0, -1, 0));
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		player->Move(0.005, XMFLOAT3(-1, 0, 0));
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		player->Move(0.005, XMFLOAT3(1, 0, 0));
	}

	/*
	if (GetAsyncKeyState(' ') & 0x8000) {
		// use ability
	}
	*/


}

// ---------- MOUSE INPUT ------------------------------------------------------

void InputManager::onMouseDown(WPARAM buttonState, int x, int y, HWND* hWnd)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(*hWnd);
}
void InputManager::onMouseUp(WPARAM buttonState, int x, int y, Camera* cam)
{
	// Add any custom code here...
	cam->MouseRotate(0, 0);

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}
void InputManager::onMouseMove(WPARAM buttonState, int x, int y, Camera* cam)
{
	// Add any custom code here...
	if (buttonState & 0x001) {
		cam->MouseRotate((y - prevMousePos.y)*.00125, (x - prevMousePos.x)*.00125);
	}
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}
void InputManager::onMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}