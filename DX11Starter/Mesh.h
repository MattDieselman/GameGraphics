#pragma once

#include <map>
#include <d3d11.h>

#include "Vertex.h"

namespace
{
	std::map<void*, int> meshRefCount;
}

class Mesh
{
public:
	Mesh(Vertex* vertices, int numVerts, unsigned int* indices, int numInds, ID3D11Device* device);
	Mesh(char* file, ID3D11Device* device);
	Mesh(const Mesh& mesh);

	Mesh* copy();
	void release();

	// Getters
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	int getIndexCount();

private:
	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	// Number of Indices contained in the mesh
	int indexCount;

	~Mesh();

	void createBuffers(Vertex* vertices, int numVerts, unsigned int* indices, int numInds, ID3D11Device* device);
};

