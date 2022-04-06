#pragma once

#include "SceneObject.h"

class CubeObject : public SceneObject
{
public:
	CubeObject(ID3D11Device*);

	ID3D11Buffer* GetVertexBuffer() const noexcept { return m_vertexBuffer.Get(); }
	ID3D11Buffer* GetIndexBuffer() const noexcept { return m_indexBuffer.Get(); }
	UINT GetElementCount() const noexcept { return m_numElements; }
	UINT GetStride() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	UINT m_numElements;
};

