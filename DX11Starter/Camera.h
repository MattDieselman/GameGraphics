#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

class Camera
{
private:
	DirectX::XMFLOAT4X4 viewMat;
	DirectX::XMFLOAT4X4 projMat;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT4 upVec;

	float xRot;
	float yRot;
public:
	DirectX::XMFLOAT4X4 getView();
	DirectX::XMFLOAT4X4 getProj();
	DirectX::XMFLOAT3 getDir();

	Camera(float width,float height);
	void Update();
	void Move(DirectX::XMFLOAT3 dir, float speed);
	void MoveYAxis(float speed);
	void ResizeCam(float width, float height);
	void MouseRotate(float x, float y);
	Camera();
	~Camera();
};

