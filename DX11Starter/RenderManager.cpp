#include "RenderManager.h"

using namespace DirectX;

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
}

std::vector<Material*> RenderManager::getMaterials()
{
	return materials;
}

void RenderManager::setSceneData(Camera * cam, DirectionalLight dirLight, PointLight pointLight, SpotLight spotLight)
{
	// Data going to Vertex Shader
	materials[0]->getVertexShader()->SetMatrix4x4("view", cam->getView());
	materials[0]->getVertexShader()->SetMatrix4x4("projection", cam->getProj());

	materials[0]->getVertexShader()->CopyAllBufferData();

	// Data going to the Pixel Shader
	bool checkDL = materials[0]->getPixelShader()->SetData(
		"dirLight",
		&dirLight,
		sizeof(DirectionalLight));
	bool checkPL = materials[0]->getPixelShader()->SetData(
		"pointLight",
		&pointLight,
		sizeof(PointLight));
	bool checkSL = materials[0]->getPixelShader()->SetData(
		"spotLight",
		&spotLight,
		sizeof(SpotLight));

	materials[0]->getPixelShader()->SetFloat3("cameraPos", cam->getPosition());

	materials[0]->getPixelShader()->CopyAllBufferData();
}

void RenderManager::setObjData(Entity * object)
{
	// Data going to Vertex Shader
	object->getMat()->getVertexShader()->SetMatrix4x4("world", object->getWorld());
	object->getMat()->getVertexShader()->CopyAllBufferData();

	// Data going to the Pixel Shader
	object->getMat()->getPixelShader()->SetShaderResourceView("diffuseTexture", object->getMat()->getTexture());
	object->getMat()->getPixelShader()->SetSamplerState("sampState", object->getMat()->getSampler());
	object->getMat()->getPixelShader()->SetShaderResourceView("normalMap", object->getMat()->getNormalMap());

	object->getMat()->getPixelShader()->CopyAllBufferData();
}


void RenderManager::LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context)
{
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.hlsl");

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))
		pixelShader->LoadShaderFile(L"PixelShader.hlsl");

	ID3D11ShaderResourceView* texture1;
	CreateWICTextureFromFile(device, context, L"textures/sphere.png", 0, &texture1);
	ID3D11ShaderResourceView* texture2;
	CreateWICTextureFromFile(device, context, L"textures/box.png", 0, &texture2);
	ID3D11ShaderResourceView* texture3;
	CreateWICTextureFromFile(device, context, L"textures/disc.png", 0, &texture3);
	ID3D11ShaderResourceView* texture4;
	CreateWICTextureFromFile(device, context, L"textures/rock.jpg", 0, &texture4);

	textures.push_back(texture1);
	textures.push_back(texture2);
	textures.push_back(texture3);
	textures.push_back(texture4);

	ID3D11ShaderResourceView* normalMap;
	CreateWICTextureFromFile(device, context, L"textures/rockNormals.jpg", 0, &normalMap);

	normalMaps.push_back(normalMap);

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampDesc, &sampler);


	materials.push_back(new Material(vertexShader, pixelShader));
	materials[0]->AttatchTexture(textures[0], sampler);
	materials[0]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[1]->AttatchTexture(textures[1], sampler);
	materials[1]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[2]->AttatchTexture(textures[2], sampler);
	materials[2]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[3]->AttatchTexture(textures[3], sampler);
	materials[3]->AttatchNormalMap(normalMaps[0]);
}

void RenderManager::DrawAll(ID3D11DeviceContext * context, float deltaTime, float totalTime, std::vector<Entity*> gameObjects, Camera * cam, ID3D11RenderTargetView* backBufferRTV,ID3D11DepthStencilView* depthStencilView)
{
	for each (Entity* object in gameObjects)
	{
		setObjData(object);

		// Send data to shader variables
		//  - Do this ONCE PER OBJECT you're drawing
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.

		//object->getMat()->getVertexShader()->SetMatrix4x4("world", object->getWorld());//entity1->world);
		//object->getMat()->getVertexShader()->SetMatrix4x4("view", cam->getView());
		//object->getMat()->getVertexShader()->SetMatrix4x4("projection", cam->getProj());
		//object->getMat()->getVertexShader()->CopyAllBufferData();
		object->getMat()->getVertexShader()->SetShader();

		// Once you've set all of the data you care to change for
		// the next draw call, you need to actually send it to the GPU
		//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!

		// Set the vertex and pixel shaders to use for the next Draw() command
		//  - These don't technically need to be set every frame...YET
		//  - Once you start applying different shaders to different objects,
		//    you'll need to swap the current shaders before each draw
		/*object->getMat()->getPixelShader()->SetShaderResourceView("diffuseTexture", object->getMat()->getTexture());
		object->getMat()->getPixelShader()->SetSamplerState("sampState", object->getMat()->getSampler());
		object->getMat()->getPixelShader()->SetFloat3("cameraPos", cam->getPosition());
		object->getMat()->getPixelShader()->CopyAllBufferData();*/
		object->getMat()->getPixelShader()->SetShader();

		// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer *verts = object->getMesh()->getVertexBuffer();
		context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
		context->IASetIndexBuffer(object->getMesh()->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			object->getMesh()->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}
}
