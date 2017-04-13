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

void InputManager::update(Entity* player, Camera* cam, float deltaTime, XMFLOAT3 worldUp)
{
	// PLAYER CONTROLS
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

	// CAMERA CONTROLS
	if (GetAsyncKeyState('X') & 0x8000) {
		cam->MoveYAxis(-.5*deltaTime);
	}
	if (GetAsyncKeyState('W') & 0x8000) {
		cam->Move(cam->getDir(), 1 * deltaTime);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		XMVECTOR temp1;
		XMFLOAT3 temp2;
		temp1 = DirectX::XMVectorNegate(XMLoadFloat3(&cam->getDir()));
		XMStoreFloat3(&temp2, temp1);
		cam->Move(temp2, 1 * deltaTime);
	}
	if (GetAsyncKeyState('A') & 0x8000) {

		XMVECTOR tempDir = XMLoadFloat3(&cam->getDir());
		XMVECTOR tempUp = XMLoadFloat3(&worldUp);
		XMFLOAT3 tempDest;
		XMStoreFloat3(&tempDest, DirectX::XMVector3Cross(tempDir, tempUp));
		cam->Move(tempDest, 1 * deltaTime);
	}
	if (GetAsyncKeyState('D') & 0x8000) {

		XMVECTOR tempDir = XMLoadFloat3(&cam->getDir());
		XMVECTOR tempUp = XMLoadFloat3(&worldUp);
		XMVECTOR tempNeg = DirectX::XMVectorNegate(DirectX::XMVector3Cross(tempDir, tempUp));
		XMFLOAT3 tempDest;
		XMStoreFloat3(&tempDest, tempNeg);
		cam->Move(tempDest, 1 * deltaTime);
	}
	
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