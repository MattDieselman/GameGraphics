#pragma once
#include <vector>
#include "Entity.h"
#include "Camera.h"

class InputManager
{
public:
	InputManager();
	~InputManager();

	void update(Entity* player);
	void onMouseDown(WPARAM buttonState, int x, int y, HWND* hWnd);
	void onMouseUp(WPARAM buttonState, int x, int y, Camera* cam);
	void onMouseMove(WPARAM buttonState, int x, int y, Camera* cam);
	void onMouseWheel(float wheelDelta, int x, int y);
	
private:
	POINT prevMousePos;
	//HWND* hWnd;
	//Camera* cam;
};

