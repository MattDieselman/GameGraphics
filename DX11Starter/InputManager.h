#pragma once
#include <vector>
#include "Entity.h"
#include "Camera.h"
#include "Emitter.h"
class InputManager
{
public:
	InputManager();
	InputManager(HWND* hWnd, Entity* player, Camera* cam, DirectX::XMFLOAT3* worldUp);
	~InputManager();

	void update(float deltaTime,std::vector<Emitter*> emitters);
	void onMouseDown(WPARAM buttonState, int x, int y, HWND* hWnd);
	void onMouseUp(WPARAM buttonState, int x, int y, Camera* cam);
	void onMouseMove(WPARAM buttonState, int x, int y, Camera* cam);
	void onMouseWheel(float wheelDelta, int x, int y);
	
private:
	POINT prevMousePos;
	HWND* hWnd;
	Camera* cam;
	DirectX::XMFLOAT3* worldUp;
	Entity* player;
};

