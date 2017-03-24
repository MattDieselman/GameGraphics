#pragma once

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

	Camera(float width,float height);
	~Camera();

	DirectX::XMFLOAT4X4 getView();
	DirectX::XMFLOAT4X4 getProj();
	DirectX::XMFLOAT3 getDir();

	void Update();
	void Move(DirectX::XMFLOAT3 dir, float speed);
	void MoveYAxis(float speed);
	void ResizeCam(float width, float height);
	void MouseRotate(float x, float y);
	
};

