#pragma once

namespace DX
{
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
		ID3D11RenderTargetView1* GetRenderTarget() const noexcept { return m_renderTarget.get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const noexcept { return m_depthStencilView.get(); }
		D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept { return m_featureLevel; }
		D3D11_VIEWPORT GetViewport() const noexcept { return m_viewport; }

		RECT GetOutputSize() const noexcept { return m_outputSize; }

		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void SetWindow(WindowHandle window, int width, int height) noexcept;

	private:
		winrt::com_ptr<ID3D11Device> m_device;
		winrt::com_ptr<ID3D11DeviceContext> m_deviceContext;

		winrt::com_ptr<IDXGIFactory2> m_dxgiFactory;
		winrt::com_ptr<IDXGIAdapter1> m_adapter;
		winrt::com_ptr<IDXGISwapChain1> m_swapChain;

		winrt::com_ptr<ID3D11Texture2D1> m_backBuffer;
		winrt::com_ptr<ID3D11Texture2D1> m_depthBuffer;

		winrt::com_ptr<ID3D11RenderTargetView1> m_renderTarget;
		winrt::com_ptr<ID3D11DepthStencilView> m_depthStencilView;

		D3D_FEATURE_LEVEL m_featureLevel;
		D3D11_VIEWPORT m_viewport;

		WindowHandle m_window;
		DXGI_FORMAT m_backBufferFormat;
		DXGI_FORMAT m_depthBufferFormat;
		RECT m_outputSize;
	};
}
