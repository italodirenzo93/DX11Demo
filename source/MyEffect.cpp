#include "pch.h"
#include "MyEffect.h"

#include "ReadData.h"

using namespace DirectX;

namespace
{
	constexpr uint32_t DirtyConstantBuffer = 0x1;
	constexpr uint32_t DirtyWVPMatrix = 0x2;
}

namespace DX
{
	MyEffect::MyEffect(ID3D11Device* device)
		:
		m_dirtyFlags(uint32_t(-1)),
		m_constantBuffer(device)
	{
		m_vsBlob = ReadData(L"VertexShader.cso");

		winrt::check_hresult(device->CreateVertexShader(m_vsBlob.data(), m_vsBlob.size(), nullptr, m_vs.put()));

		auto psBlob = ReadData(L"PixelShader.cso");

		winrt::check_hresult(device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, m_ps.put()));

		m_commonStates = std::make_unique<CommonStates>(device);
	}

	void MyEffect::Apply(ID3D11DeviceContext* deviceContext)
	{
		if (m_dirtyFlags & DirtyWVPMatrix)
		{
			m_worldViewProj = m_world * m_view * m_proj;

			m_dirtyFlags &= ~DirtyWVPMatrix;
			m_dirtyFlags |= DirtyConstantBuffer;
		}

		if (m_dirtyFlags & DirtyConstantBuffer)
		{
			MyEffectConstants constants = {};
			//constants.worldViewProj = XMMatrixTranspose(m_worldViewProj);
			constants.worldViewProj = m_worldViewProj;
			m_constantBuffer.SetData(deviceContext, constants);

			m_dirtyFlags &= ~DirtyConstantBuffer;
		}

		const auto cb = m_constantBuffer.GetBuffer();
		deviceContext->VSSetConstantBuffers(0, 1, &cb);

		const auto sr = m_texture.get();
		deviceContext->PSSetShaderResources(0, 1, &sr);

		const auto samplers = m_commonStates->PointClamp();
		deviceContext->PSSetSamplers(0, 1, &samplers);

		deviceContext->VSSetShader(m_vs.get(), nullptr, 0);
		deviceContext->PSSetShader(m_ps.get(), nullptr, 0);
	}

	void MyEffect::GetVertexShaderBytecode(void const** pShaderBytecode, size_t* pBytecodeLength)
	{
		assert(pShaderBytecode != nullptr && pBytecodeLength != nullptr);
		*pShaderBytecode = m_vsBlob.data();
		*pBytecodeLength = m_vsBlob.size();
	}

	void XM_CALLCONV MyEffect::SetWorld(DirectX::FXMMATRIX value)
	{
		m_world = value;
		m_dirtyFlags |= DirtyWVPMatrix;
	}

	void XM_CALLCONV MyEffect::SetView(DirectX::FXMMATRIX value)
	{
		m_view = value;
		m_dirtyFlags |= DirtyWVPMatrix;
	}

	void XM_CALLCONV MyEffect::SetProjection(DirectX::FXMMATRIX value)
	{
		m_proj = value;
		m_dirtyFlags |= DirtyWVPMatrix;
	}

	void XM_CALLCONV MyEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
	{
		m_world = world;
		m_view = view;
		m_proj = projection;
		m_dirtyFlags |= DirtyWVPMatrix;
	}

	void MyEffect::SetTexture(const winrt::com_ptr<ID3D11ShaderResourceView>& texture)
	{
		m_texture = texture;
	}
}
