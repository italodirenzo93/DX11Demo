#include "pch.h"
#include "CubeObject.h"

#include <GeometricPrimitive.h>

#include "MyEffect.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DX
{
	using VertexType = MyEffect::VertexType;

	CubeObject::CubeObject(ID3D11Device* device)
		:
		CubeObject(device, Vector3::Zero)
	{
	}

	CubeObject::CubeObject(ID3D11Device* device, const Vector3& position)
		:
		SceneObject(position)
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
			CreateStaticBuffer(device, newVerts, D3D11_BIND_VERTEX_BUFFER, m_vertexBuffer.put())
		);

		ThrowIfFailed(
			CreateStaticBuffer(device, indices, D3D11_BIND_INDEX_BUFFER, m_indexBuffer.put())
		);

		m_numElements = static_cast<UINT>(indices.size());
	}

	UINT CubeObject::GetStride() const noexcept
	{
		return static_cast<UINT>(sizeof(VertexType));
	}
}
