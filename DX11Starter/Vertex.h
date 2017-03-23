#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct ColorVertex
{
	DirectX::XMFLOAT3 Position;	    // The position of the vertex
	DirectX::XMFLOAT4 Color;        // The color of the vertex
	//DirectX::XMFLOAT2 UV;
	//DirectX::XMFLOAT3 Normal;
};
struct Vertex {
	DirectX::XMFLOAT3 position;	    // The position of the vertex
	//DirectX::XMFLOAT4 color;      // The color of the vertex
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
};