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

		ID3D11Device5* GetD3DDevice() const noexcept;
		ID3D11DeviceContext4* GetD3DDeviceContext() const noexcept;

		ID2D1Factory1* GetD2DFactory() const noexcept { return m_d2dFactory.Get(); }
		ID2D1Device* GetD2DDevice() const noexcept { return m_d2dDevice.Get(); }
		ID2D1DeviceContext* GetD2DDeviceContext() const noexcept { return m_d2dDeviceContext.Get(); }

		IDWriteFactory7* GetDWriteFactory() const noexcept { return m_dWriteFactory.Get(); }

		IDXGISwapChain4* GetSwapChain() const noexcept;
		IDXGIAdapter4* GetDXGIAdapter() const noexcept;
		IDXGIFactory7* GetDXGIFactory() const noexcept;

		ID3D11RenderTargetView1* GetRenderTargetView() const noexcept { return m_renderTarget.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const noexcept { return m_depthStencilView.Get(); }
		D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept { return m_featureLevel; }
		D3D11_VIEWPORT GetViewport() const noexcept { return m_viewport; }

		RECT GetOutputSize() const noexcept { return m_outputSize; }

		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void SetWindow(WindowHandle window, int width, int height) noexcept;

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceDependentResources();

		wrl::ComPtr<ID3D11Device> m_d3dDevice;
		wrl::ComPtr<ID3D11DeviceContext> m_d3dDeviceContext;
		wrl::ComPtr<ID3D11Texture2D1> m_backBuffer;
		wrl::ComPtr<ID3D11Texture2D1> m_depthBuffer;
		wrl::ComPtr<ID3D11RenderTargetView1> m_renderTarget;
		wrl::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		wrl::ComPtr<ID2D1Factory1> m_d2dFactory;
		wrl::ComPtr<ID2D1Device> m_d2dDevice;
		wrl::ComPtr<ID2D1DeviceContext> m_d2dDeviceContext;
		wrl::ComPtr<ID2D1Bitmap1> m_d2dBitmap;

		wrl::ComPtr<IDXGIFactory2> m_dxgiFactory;
		wrl::ComPtr<IDXGIAdapter1> m_adapter;
		wrl::ComPtr<IDXGISwapChain1> m_swapChain;

		wrl::ComPtr<IDWriteFactory7> m_dWriteFactory;

		D3D_FEATURE_LEVEL m_featureLevel;
		D3D11_VIEWPORT m_viewport;

		WindowHandle m_window;
		DXGI_FORMAT m_backBufferFormat;
		DXGI_FORMAT m_depthBufferFormat;
		RECT m_outputSize;
	};
}
