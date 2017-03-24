#include "Camera.h"

using namespace DirectX;

// Constructors / Destructors

Camera::Camera(float width,float height)
{
	position = XMFLOAT3(1, 1, -10);
	direction = XMFLOAT3(0, 0, 1);
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	upVec = XMFLOAT4(0, 1, 0, 0);
	xRot = 0;
	yRot = 0;
	XMVECTOR up = XMLoadFloat4(&upVec);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMat, XMMatrixTranspose(V));

	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projMat, XMMatrixTranspose(P));
}

Camera::~Camera()
{
}

// Gets / Sets

XMFLOAT4X4 Camera::getView()
{
	return viewMat;
}

XMFLOAT4X4 Camera::getProj()
{
	return projMat;
}

XMFLOAT3 Camera::getDir()
{
	return direction;
}

// Methods

void Camera::Update()
{	
	XMVECTOR dir = XMLoadFloat3(&direction);
	dir = XMVector3Rotate(dir, XMQuaternionRotationRollPitchYaw(xRot, yRot, 0));
	XMMATRIX V = XMMatrixLookToLH(XMLoadFloat3(&position),dir,XMLoadFloat4(&upVec));
	XMStoreFloat4x4(&viewMat, XMMatrixTranspose(V));
	XMStoreFloat3(&direction, dir);
}

void Camera::Move(XMFLOAT3 dir, float speed)
{
	//XMVECTOR dir = XMVector3Rotate(XMVectorSet(direction.x,direction.y,direction.z,0),XMLoadFloat4(&dir)
	//pos = pos + (1* XMLoadFloat(&speed));
	//XMVECTOR temp;
	dir.x = dir.x*speed;
	dir.y = dir.y*speed;
	dir.z = dir.z*speed;

	//temp = XMVectorMultiply(XMLoadFloat3(&dir), XMLoadFloat(&speed));
	XMStoreFloat3(&position, XMLoadFloat3(&position)+ XMLoadFloat3(&dir));
}


void Camera::MoveYAxis(float speed)
{
		position.y+=speed;
 }


void Camera::ResizeCam(float width, float height)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projMat, XMMatrixTranspose(P));
}

void Camera::MouseRotate(float x, float y)
{
	if (x > -1 && x < 1)xRot = x;
	else if (x < -1) xRot = -1;
	else if (x > 1) xRot = 1;
	//else xRot = 0;
	if (y > -1 && y < 1)yRot = y;
	else if (y < -1) yRot = -1;
	else if (y > 1) yRot = 1;
}
