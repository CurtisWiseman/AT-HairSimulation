#pragma once

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
#include <vector>

class LineClass
{
public:
	struct ModelType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

private: 
	struct PointType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	LineClass();
	LineClass(std::vector<XMFLOAT3> _parent_positions, std::vector<XMFLOAT3> _parent_normals, int _parent_count);
	LineClass(const LineClass&);
	~LineClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, float delta_time);
	int GetIndexCount();
	void UpdateBuffers(ID3D11DeviceContext*, float delta_time);

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	std::vector<ModelType> parent;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	const int m_vertex_count_per_hair = 20, m_index_count_per_hair = 20;
	int m_parent_count;
	const float m_hair_length = 0.2f;
	PointType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	const float use_physics = true;
};