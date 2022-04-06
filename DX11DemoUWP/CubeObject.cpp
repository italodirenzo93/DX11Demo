#include "pch.h"
#include "CubeObject.h"

#include <GeometricPrimitive.h>

using namespace DX;
using namespace DirectX;
using namespace DirectX::SimpleMath;

struct VertexType
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector2 tex;
};

CubeObject::CubeObject(ID3D11Device* device)
{
	GeometricPrimitive::VertexCollection vertices;
	GeometricPrimitive::IndexCollection indices;
	GeometricPrimitive::CreateCube(vertices, indices);

	std::vector<VertexType> newVerts;
	newVerts.reserve(vertices.size());
	for (const auto& v : vertices)
	{
		VertexType vert;
		vert.position = v.position;
		vert.tex = v.textureCoordinate;
		newVerts.emplace_back(vert);
	}

	ThrowIfFailed(
		CreateStaticBuffer(device, newVerts, D3D11_BIND_VERTEX_BUFFER, m_vertexBuffer.ReleaseAndGetAddressOf())
	);

	ThrowIfFailed(
		CreateStaticBuffer(device, indices, D3D11_BIND_INDEX_BUFFER, m_indexBuffer.ReleaseAndGetAddressOf())
	);

	m_numElements = static_cast<UINT>(indices.size());
}

UINT CubeObject::GetStride() const noexcept
{
	return static_cast<UINT>(sizeof(VertexType));
}
