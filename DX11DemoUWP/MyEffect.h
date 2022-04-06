#pragma once

class MyEffect : public DirectX::IEffect, DirectX::IEffectMatrices
{
public:
	explicit MyEffect(ID3D11Device* device);

	virtual void Apply(ID3D11DeviceContext* deviceContext) override;
	virtual void GetVertexShaderBytecode(void const** pShaderBytecode, size_t* pBytecodeLength) override;

	void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

	void SetTexture(ID3D11ShaderResourceView* texture);

private:
	std::unique_ptr<DirectX::CommonStates> m_commonStates;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	std::vector<uint8_t> m_vsBlob;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;
	DirectX::SimpleMath::Matrix m_worldViewProj;

	uint32_t m_dirtyFlags;

	struct __declspec(align(16)) MyEffectConstants
	{
		DirectX::XMMATRIX worldViewProj;
	};

	DirectX::ConstantBuffer<MyEffectConstants> m_constantBuffer;

public:
	static inline constexpr D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static inline constexpr UINT InputLayoutCount = _countof(InputLayout);
};

