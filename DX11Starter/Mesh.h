#pragma once
#include "DXCore.h"
#include <d3d11.h>
#include "Vertex.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
class Mesh
{
private:
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;
	ID3D11Device * device;
	int numIndicies;
public:
	Mesh();
	Mesh(char* objFile, ID3D11Device * dev);
	Mesh( Vertex * verts, unsigned int * indexes, ID3D11Device * dev,int numVerts,int numIndicies);
	void BufferHelper(Vertex * verts, unsigned int * indexes, ID3D11Device * dev, int numVerts, int numIndicies);
	ID3D11Buffer * GetVertexBuffer();
	ID3D11Buffer * GetIndexBuffer();
	int getNumIndicies();
	~Mesh();
};

