#pragma once

#include "SceneObject.h"

namespace DX
{
	class CubeObject : public SceneObject
	{
	public:
		explicit CubeObject(ID3D11Device* device);
		CubeObject(ID3D11Device* device, const DirectX::SimpleMath::Vector3& position);

		ID3D11Buffer* GetVertexBuffer() const noexcept { return m_vertexBuffer.get(); }
		ID3D11Buffer* GetIndexBuffer() const noexcept { return m_indexBuffer.get(); }
		UINT GetElementCount() const noexcept { return m_numElements; }
		UINT GetStride() const noexcept;

	private:
		winrt::com_ptr<ID3D11Buffer> m_vertexBuffer;
		winrt::com_ptr<ID3D11Buffer> m_indexBuffer;
		UINT m_numElements;
	};
}
