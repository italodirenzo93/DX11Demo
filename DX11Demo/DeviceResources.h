#pragma once

class DeviceResources
{
public:
	DeviceResources(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT);
	~DeviceResources();

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PC_APP)
	using WindowHandle = ::IUnknown*;
#else
	using WindowHandle = HWND;
#endif

	ID3D11Device5* GetDevice() const noexcept;
	ID3D11DeviceContext4* GetDeviceContext() const noexcept;
	IDXGISwapChain4* GetSwapChain() const noexcept;
	IDXGIAdapter4* GetAdapter() const noexcept;
	IDXGIFactory5* GetFactory() const noexcept;
	ID3D11RenderTargetView1* GetRenderTarget() const noexcept { return m_renderTarget.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const noexcept { return m_depthStencilView.Get(); }
	D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept { return m_featureLevel; }
	DirectX::SimpleMath::Viewport GetViewport() const noexcept { return m_viewport; }

	RECT GetOutputSize() const noexcept { return m_outputSize; }

	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void SetWindow(WindowHandle window, int width, int height) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;

	Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D1> m_backBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D1> m_depthBuffer;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1> m_renderTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	D3D_FEATURE_LEVEL m_featureLevel;
	DirectX::SimpleMath::Viewport m_viewport;

	WindowHandle m_window;
	DXGI_FORMAT m_backBufferFormat;
	DXGI_FORMAT m_depthBufferFormat;
	RECT m_outputSize;
};

