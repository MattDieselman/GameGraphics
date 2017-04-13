#pragma once
#include <vector>
#include "Entity.h"
#include "Camera.h"
#include "DXCore.h"

class InputManager
{
public:
	InputManager();
	~InputManager();

	void update(Entity* player, Camera* cam, float deltaTime, DirectX::XMFLOAT3 worldUp);
	void onMouseDown(WPARAM buttonState, int x, int y, HWND* hWnd);
	void onMouseUp(WPARAM buttonState, int x, int y, Camera* cam);
	void onMouseMove(WPARAM buttonState, int x, int y, Camera* cam);
	void onMouseWheel(float wheelDelta, int x, int y);
	
private:
	POINT prevMousePos;
	//HWND* hWnd; // error
	//Camera* cam;
	//XMFLOAT3* worldUp; //override error?
	//Entity* player
};

