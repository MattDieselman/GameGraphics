#pragma once

#include <unordered_map>
#include <d3d11.h>

#include "Vertex.h"

namespace
{
	std::unordered_map<void*, int> meshRefCount;
}

// static std::map<void*, int> meshRefCount;

extern class Entity;

class Mesh
{
public:
	Mesh();
	Mesh(Vertex* vertices, int numVerts, unsigned int* indices, int numInds, ID3D11Device* device);
	Mesh(char* file, ID3D11Device* device);
	Mesh(const Mesh& mesh);

	/*void CreateMesh(Vertex* vertices, int numVerts, unsigned int* indices, int numInds, ID3D11Device* device);
	void LoadMesh(char* objFile, ID3D11Device* device);*/

	Mesh* copy();
	void release();

	// Getters
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	std::vector<Vertex> getVertices();
	int getVertexCount();
	int getIndexCount();

private:
	~Mesh();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Number of Indices contained in the mesh
	int indexCount;
	int vertexCount;

	std::vector<Vertex> vertices;

	void createBuffers(Vertex* vertices, int numVerts, unsigned int* indices, int numInds, ID3D11Device* device);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};