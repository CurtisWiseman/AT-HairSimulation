#include "modelclass.h"
#include <assimp\mesh.h>

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{

	//Load the model.
	if (!LoadModel(modelFilename))
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	if(!InitializeBuffers(device))
	{
		return false;
	}

	// Load the texture for this model.
	if (!LoadTexture(device, deviceContext, textureFilename))
	{
		return false;
	}

	if (!InitializeHairs(device, deviceContext))
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Release the model texture.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

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

	//Release the model object
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	//Release the control hairs
	if (m_control_hairs)
	{
		m_control_hairs->Shutdown();
		delete m_control_hairs;
		m_control_hairs = 0;
	}

	m_vertexCount = 0;
	m_indexCount = 0;

	m_importer.FreeScene();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext, float delta_time)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


void ModelClass::RenderHairs(ID3D11DeviceContext* deviceContext, float delta_time)
{
	m_control_hairs->Render(deviceContext, delta_time);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


int ModelClass::GetHairIndexCount()
{
	return m_control_hairs->GetIndexCount();
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex and index arrays with data.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = m_model[i].position;
		vertices[i].texture = m_model[i].texture;
		vertices[i].normal = m_model[i].normal;

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
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
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass();
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}


bool ModelClass::LoadModel(char* filename)
{
	if (use_assimp)
	{
		m_scene = *m_importer.ReadFile(filename, NULL);

		if (m_scene.HasMeshes())
		{
			aiMesh* t = m_scene.mMeshes[0];
			m_vertexCount = t->mNumVertices;

			m_indexCount = m_vertexCount;

			m_model = new ModelType[m_vertexCount];
			if (!m_model)
			{
				return false;
			}

			for (int i = 0; i < m_vertexCount; i++)
			{
				m_model[i].position.x = t->mVertices[i].x;
				m_model[i].position.y = t->mVertices[i].y;
				m_model[i].position.z = t->mVertices[i].z;

				if (t->HasTextureCoords(i))
				{
					m_model[i].texture.x = t->mTextureCoords[i]->x;
					m_model[i].texture.y = t->mTextureCoords[i]->y;
				}
				else
				{
					m_model[i].texture.x = 0;
					m_model[i].texture.y = 0;
				}

				if (t->HasNormals())
				{
					m_model[i].normal.x = t->mNormals[i].x;
					m_model[i].normal.y = t->mNormals[i].y;
					m_model[i].normal.z = t->mNormals[i].z;
				}
				else
				{
					m_model[i].normal.x = 0.5f;
					m_model[i].normal.y = 0.5f;
					m_model[i].normal.z = 0.5f;
				}
			}
		}
	}
	else
	{
		ifstream fin;
		char input;
		int i;

		fin.open(filename);
		if (fin.fail())
		{
			return false;
		}

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		fin >> m_vertexCount;
		m_indexCount = m_vertexCount;

		m_model = new ModelType[m_vertexCount];
		if (!m_model)
		{
			return false;
		}

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		for (i = 0; i<m_vertexCount; i++)
		{
			fin >> m_model[i].position.x >> m_model[i].position.y >> m_model[i].position.z;
			fin >> m_model[i].texture.x >> m_model[i].texture.y;
			fin >> m_model[i].normal.x >> m_model[i].normal.y >> m_model[i].normal.z;
		}

		fin.close();
	}

	return true;
}


bool ModelClass::InitializeHairs(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
	int hair_num = 0;
	std::vector<XMFLOAT3> t_positions;
	std::vector<XMFLOAT3> t_normals;
	t_positions.resize(m_vertexCount);
	t_normals.resize(m_vertexCount);
	for (int i = 0; i < m_vertexCount; i++)
	{
		//If texture coordinates m_model[i].tex are (black) then push back a new tpos and tnorm, haircount++, otherwise skip
		t_positions[i] = m_model[i].position;
		t_normals[i] = m_model[i].normal;
		hair_num++;
	}
	m_control_hairs = new LineClass(t_positions, t_normals, m_vertexCount);

	m_control_hairs->Initialize(_device, _deviceContext);

	return true;
}