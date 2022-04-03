#pragma once

#include "StepTimer.h"
#include "DeviceResources.h"

struct VS_CONSTANT_BUFFER
{
	DirectX::XMMATRIX matWorldViewProj;
};

class SampleScene
{
public:
	SampleScene();
	~SampleScene();

	void Initialize(::IUnknown* window, int width, int height);
	void OnWindowSizeChanged(int width, int height);
	void Tick();

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	void Update(const DX::StepTimer& timer);
	void Render();

	DX::StepTimer m_timer;

	std::unique_ptr<DX::DeviceResources> m_deviceResources;

	std::unique_ptr<DirectX::CommonStates> m_commonStates;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	DirectX::ConstantBuffer<VS_CONSTANT_BUFFER> m_constantBuffer;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_projection;

	::IUnknown* m_window;
};

