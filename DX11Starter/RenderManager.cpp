#include "RenderManager.h"

using namespace DirectX;

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
	// Clean Up Shadows
	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();
	delete shadowVertexShader;
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
	materials[0]->getVertexShader()->SetMatrix4x4("shadowView", shadowViewMatrix);
	materials[0]->getVertexShader()->SetMatrix4x4("shadowProj", shadowProjectionMatrix);

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
	materials[0]->getPixelShader()->SetSamplerState("ShadowSampler", shadowSampler);
	materials[0]->getPixelShader()->SetShaderResourceView("ShadowMap", shadowSRV);

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

SimpleVertexShader * RenderManager::getPartVert()
{
	return partVertexShader;
}

SimplePixelShader * RenderManager::getPartPix()
{
	return partPixelShader;
}

ID3D11ShaderResourceView* RenderManager::getPartText(int index)
{
	return particleTextures[index];
}

void RenderManager::InitShadows(ID3D11Device * device, ID3D11DeviceContext * context)
{
	// Spot Light Matrices
	XMMATRIX shView = XMMatrixLookToLH(
		XMVectorSet(2, 0, 0, 0),	// Light position
		XMVectorSet(-1, 0, 0, 0),	// Light direction
		XMVectorSet(0, 1, 0, 0));	// Up direction
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shView));
	XMMATRIX shProj = XMMatrixOrthographicLH(
		10.0f,		// Width
		10.0f,		// Height
		0.1f,		// Near clip
		50.0f);		// Far clip
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shProj));

	// Texture2D
	shadowMapSize = 1024;
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	// Release the texture reference since we don't need it
	shadowTexture->Release();

	// special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Rasterizer State
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);
}

void RenderManager::RenderShadowMap(ID3D11DeviceContext * context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView * backBufferRTV, ID3D11DepthStencilView * depthStencilView, unsigned int * width, unsigned int * height)
{
	// Set up the render targets and depth buffer (shadow map) and
	// other pipeline states
	context->OMSetRenderTargets(0, 0, shadowDSV);
	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)shadowMapSize;
	vp.Height = (float)shadowMapSize;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	// Set up the vertex shader for shadow rendering
	shadowVertexShader->SetShader();
	shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix);
	shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);

	// Turn off the pixel shader
	context->PSSetShader(0, 0, 0);

	// Render all of our entities to the shadow map
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for each (Entity* object in *gameObjects)
	{
		// Set buffers in the input assembler
		ID3D11Buffer* vb = object->getMesh()->getVertexBuffer();
		ID3D11Buffer* ib = object->getMesh()->getIndexBuffer();
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shadowVertexShader->SetMatrix4x4("world", object->getWorld());
		shadowVertexShader->CopyAllBufferData();
		
		// Draw into shadow map
		context->DrawIndexed(object->getMesh()->getIndexCount(), 0, 0);
	}

	// Revert all the DX settings for "regular" drawing
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	vp.Width = (float)*width;
	vp.Height = (float)*height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0); // Restores default state
}


void RenderManager::LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context)
{

	//Particle Shader Loading
	partVertexShader = new SimpleVertexShader(device, context);
	partPixelShader = new SimplePixelShader(device, context);


	if (!partVertexShader->LoadShaderFile(L"Debug/PartVertexShader.cso"))
		partVertexShader->LoadShaderFile(L"PartVertexShader.hlsl");

	if (!partPixelShader->LoadShaderFile(L"Debug/PartPixelShader.cso"))
		partPixelShader->LoadShaderFile(L"PartPixelShader.hlsl");

	shadowVertexShader = new SimpleVertexShader(device, context);
	if (!shadowVertexShader->LoadShaderFile(L"Debug/ShadowVertexShader.cso"))
		shadowVertexShader->LoadShaderFile(L"ShadowVertexShader.hlsl");

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
	CreateWICTextureFromFile(device, context, L"textures/rock.jpg", 0, &texture4);;

	ID3D11ShaderResourceView* particleTexture;
	ID3D11ShaderResourceView* particleTexture2;

	CreateWICTextureFromFile(device, context, L"textures/fireParticle.jpg", 0, &particleTexture);
	CreateWICTextureFromFile(device, context, L"textures/particle.jpg", 0, &particleTexture2);

	particleTextures.push_back(particleTexture);
	particleTextures.push_back(particleTexture2);

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

	//Particle depth and blend states
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);

	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);


	//Create Emitter for parts

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


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

void RenderManager::DrawAll(ID3D11DeviceContext * context, float deltaTime, float totalTime, std::vector<Entity*> gameObjects, Camera * cam, std::vector<Emitter*> emitters, ID3D11RenderTargetView* backBufferRTV,ID3D11DepthStencilView* depthStencilView)
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


	//Draw Emitter & parts


	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING
	for each (Emitter * e in emitters)
		e->DrawAll(context, cam);

	//reset drawstates
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
	
}
