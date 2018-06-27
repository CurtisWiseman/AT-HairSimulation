#include "lineclass.h"

//Float3 operators because they don't exist yet
XMFLOAT3 operator+(XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	XMFLOAT3 xhs;
	xhs.x = lhs.x + rhs.x;
	xhs.y = lhs.y + rhs.y;
	xhs.z = lhs.z + rhs.z;
	return xhs;
}
XMFLOAT3 operator-(XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	XMFLOAT3 xhs;
	xhs.x = lhs.x - rhs.x;
	xhs.y = lhs.y - rhs.y;
	xhs.z = lhs.z - rhs.z;
	return xhs;
}
XMFLOAT3 operator+=(XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}
XMFLOAT3 operator-=(XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}
XMFLOAT3 operator*(XMFLOAT3& lhs, float& rhs)
{
	XMFLOAT3 xhs;
	xhs.x = lhs.x * rhs;
	xhs.y = lhs.y * rhs;
	xhs.z = lhs.z * rhs;
	return xhs;
}




LineClass::LineClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


LineClass::LineClass(std::vector<XMFLOAT3> _parent_positions, std::vector<XMFLOAT3> _parent_normals, int _parent_count)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_parent_count = _parent_count;
	parent.resize(m_parent_count);
	for (int i = 0; i < m_parent_count; i++)
	{
		parent[i].position = _parent_positions[i];
		parent[i].normal = _parent_normals[i];
		parent[i].texture = XMFLOAT2(0, 0);
	}
}


LineClass::LineClass(const LineClass& other)
{
}


LineClass::~LineClass()
{
}


bool LineClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}


void LineClass::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	// Release vertices
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}

	// Release indices
	if (indices)
	{
		delete[] indices;
		indices = 0;
	}

	// Release parent object
	parent.clear();
	parent.empty();

	return;
}


void LineClass::Render(ID3D11DeviceContext* deviceContext, float delta_time)
{
	UpdateBuffers(deviceContext, delta_time);
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int LineClass::GetIndexCount()
{
	return m_index_count_per_hair * m_parent_count;
}


bool LineClass::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;

	// Create the vertex array.
	vertices = new PointType[m_vertex_count_per_hair * m_parent_count];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_index_count_per_hair * m_parent_count];
	if (!indices)
	{
		return false;
	}

	// Load the vertex and index arrays with data.
	for (int j = 0; j < m_parent_count; j++)
	{
		for (int i = 0; i < m_vertex_count_per_hair; i++)
		{
			int modOffsetVal = (i / 2) + (i % 2);
			float posX = parent[j].position.x + parent[j].normal.x * (float)modOffsetVal * (m_hair_length / m_vertex_count_per_hair);
			float posY = parent[j].position.y + parent[j].normal.y * (float)modOffsetVal * (m_hair_length / m_vertex_count_per_hair);
			float posZ = parent[j].position.z + parent[j].normal.z * (float)modOffsetVal * (m_hair_length / m_vertex_count_per_hair);
			vertices[(j*m_vertex_count_per_hair) + i].position = XMFLOAT3(posX, posY, posZ);
			vertices[(j*m_vertex_count_per_hair) + i].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			indices[(j*m_vertex_count_per_hair) + i] = (j*m_vertex_count_per_hair) + i;
		}
	}
	

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(PointType) * m_vertex_count_per_hair * m_parent_count;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_index_count_per_hair * m_parent_count;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete[] vertices;
	//vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void LineClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(PointType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}


void LineClass::UpdateBuffers(ID3D11DeviceContext* _devcon, float delta_time)
{
	XMFLOAT3 v_pos;
	XMFLOAT3 v_minus_1_pos;
	XMFLOAT3 v_minus_2_pos;
	XMFLOAT3 diff;

	for (int j = 0; j < m_parent_count; j++)
	{
		if (m_vertex_count_per_hair > 1)
		{
			for (int i = 1; i < m_vertex_count_per_hair; i++)
			{
				v_pos = vertices[j*m_vertex_count_per_hair + i].position;
				v_minus_1_pos = vertices[j*m_vertex_count_per_hair + i - 1].position;
				v_minus_2_pos = vertices[j*m_vertex_count_per_hair + i - 2].position;

				
				if (use_physics)
				{
					//v_pos is physically linked to v_minus_1_pos, using deltatime

					//Move it down by v.small amount (gravity)
					v_pos.y -= 0.1f * delta_time;

					//Spring it back toward the parent vertex
					if ((i % 2))
					{
						diff = v_pos - v_minus_1_pos;
					}
					else
					{
						diff = v_pos - v_minus_2_pos;
					}

					float distance_sqrd = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
					if (distance_sqrd > ((m_hair_length / m_vertex_count_per_hair) * m_hair_length))
					{
						v_pos -= (diff * delta_time);
					}
				}
				else
				{
					v_pos = v_minus_1_pos;
					v_pos.y -= (m_hair_length / m_vertex_count_per_hair);
				}

				vertices[j*m_vertex_count_per_hair + i].position = v_pos;
				vertices[j*m_vertex_count_per_hair + i - 1].position = v_minus_1_pos;
			}
		}
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	_devcon->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, vertices, sizeof(PointType) * m_vertex_count_per_hair * m_parent_count);
	_devcon->Unmap(m_vertexBuffer, 0);
}