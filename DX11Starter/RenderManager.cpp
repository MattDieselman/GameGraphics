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


void RenderManager::LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height)
{
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.hlsl");

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))
		pixelShader->LoadShaderFile(L"PixelShader.hlsl");

	ppVS = new SimpleVertexShader(device, context);
	if (!ppVS->LoadShaderFile(L"Debug/PostProcessVS.cso"))
		ppVS->LoadShaderFile(L"PostProcessVS.cso");

	ppPS = new SimplePixelShader(device, context);
	if (!ppPS->LoadShaderFile(L"Debug/PostProcessPS.cso"))
		ppPS->LoadShaderFile(L"PostProcessPS.cso");

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

	// Sampler state for texture sampling
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampDesc, &sampler);

	ppSampler = sampler;

	// Create post process resources -----------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* ppTexture;
	device->CreateTexture2D(&textureDesc, 0, &ppTexture);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(ppTexture, &rtvDesc, &ppRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	device->CreateShaderResourceView(ppTexture, &srvDesc, &ppSRV);

	// We don't need the texture reference itself anymore
	ppTexture->Release();

	// Create and push materials
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

void RenderManager::DrawAll(ID3D11DeviceContext * context, float deltaTime, float totalTime, std::vector<Entity*> gameObjects, Camera * cam, ID3D11RenderTargetView* backBufferRTV,ID3D11DepthStencilView* depthStencilView, unsigned int width, unsigned int height)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set up the post process render target =======================
	context->OMSetRenderTargets(1, &ppRTV, depthStencilView);
	context->ClearRenderTargetView(ppRTV, color);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (Entity* object : gameObjects)
	{
		setObjData(object);

		object->getMat()->getVertexShader()->SetShader();

		object->getMat()->getPixelShader()->SetShader();

		// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		ID3D11Buffer *verts = object->getMesh()->getVertexBuffer();
		context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
		context->IASetIndexBuffer(object->getMesh()->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			object->getMesh()->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	// Get ready for post processing ====================
	context->OMSetRenderTargets(1, &backBufferRTV, 0);

	// Turn on VS (no other data necessary)
	ppVS->SetShader();

	// Turn on PS
	ppPS->SetShader();
	ppPS->SetShaderResourceView("Pixels", ppSRV);
	ppPS->SetSamplerState("Sampler", ppSampler);
	ppPS->SetFloat("pixelWidth", 1.0f / width);
	ppPS->SetFloat("pixelHeight", 1.0f / height);
	ppPS->SetInt("blurAmount", 5);
	ppPS->CopyAllBufferData();

	// Turn off vertex and index buffers
	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw the post process (3 verts = 1 triangle to fill the screen)
	context->Draw(3, 0);

	// Unbind the post process SRV
	ppPS->SetShaderResourceView("Pixels", 0);
}
