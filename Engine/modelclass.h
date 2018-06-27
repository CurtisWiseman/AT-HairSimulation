#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_
#pragma once

#include "textureclass.h"
#include "lineclass.h"
#include <vector>
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
#include <fstream>
using namespace std;

#pragma comment(lib, "x86/assimp.lib")
#include <assimp\Importer.hpp>
#include <assimp\scene.h>

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, float delta_time);
	void RenderHairs(ID3D11DeviceContext*, float delta_time);
	int GetIndexCount();
	int GetHairIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool LoadModel(char*);
	bool InitializeHairs(ID3D11Device*, ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	aiScene m_scene;
	ModelType* m_model;
	LineClass* m_control_hairs;
	Assimp::Importer m_importer;
	const bool use_assimp = true;
};

#endif