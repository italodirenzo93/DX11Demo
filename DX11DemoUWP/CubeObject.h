#pragma once

#include "SceneObject.h"

class CubeObject : public SceneObject
{
public:
	explicit CubeObject(ID3D11Device* device);
	CubeObject(ID3D11Device* device, const DirectX::SimpleMath::Vector3& position);

	ID3D11Buffer* GetVertexBuffer() const noexcept { return m_vertexBuffer.Get(); }
	ID3D11Buffer* GetIndexBuffer() const noexcept { return m_indexBuffer.Get(); }
	UINT GetElementCount() const noexcept { return m_numElements; }
	UINT GetStride() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	UINT m_numElements;
};

