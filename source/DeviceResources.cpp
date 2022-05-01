#include "pch.h"
#include "DeviceResources.h"

using Microsoft::WRL::ComPtr;

namespace DX
{
	DeviceResources::DeviceResources(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat)
		:
		m_backBufferFormat(backBufferFormat),
		m_depthBufferFormat(depthBufferFormat),
		m_featureLevel(D3D_FEATURE_LEVEL_1_0_CORE),
		m_window(nullptr),
		m_outputSize(),
		m_viewport()
	{
	}

	DeviceResources::~DeviceResources()
	{
	}

	ID3D11Device5* DeviceResources::GetD3DDevice() const noexcept
	{
		ComPtr<ID3D11Device5> pDevice5;
		m_d3dDevice.As(&pDevice5);
		return pDevice5.Get();
	}

	ID3D11DeviceContext4* DeviceResources::GetD3DDeviceContext() const noexcept
	{
		ComPtr<ID3D11DeviceContext4> pDeviceContext4;
		m_d3dDeviceContext.As(&pDeviceContext4);
		return pDeviceContext4.Get();
	}

	IDXGISwapChain4* DeviceResources::GetSwapChain() const noexcept
	{
		ComPtr<IDXGISwapChain4> pSwapChain4;
		m_swapChain.As(&pSwapChain4);
		return pSwapChain4.Get();
	}

	IDXGIAdapter4* DeviceResources::GetDXGIAdapter() const noexcept
	{
		ComPtr<IDXGIAdapter4> pAdapter4;
		m_adapter.As(&pAdapter4);
		return pAdapter4.Get();
	}

	IDXGIFactory7* DeviceResources::GetDXGIFactory() const noexcept
	{
		ComPtr<IDXGIFactory7> pFactory7;
		m_dxgiFactory.As(&pFactory7);
		return pFactory7.Get();
	}

	void DeviceResources::SetWindow(WindowHandle window, int width, int height) noexcept
	{
		m_window = window;

		m_outputSize.left = m_outputSize.top = 0;
		m_outputSize.right = width;
		m_outputSize.bottom = height;
	}

	void DeviceResources::CreateDeviceResources()
	{
		CreateDeviceIndependentResources();
		CreateDeviceDependentResources();
	}

	void DeviceResources::CreateDeviceIndependentResources()
	{
		// Create DXGI essentials
		UINT dxgiFlags = 0;
#ifdef _DEBUG
		dxgiFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG


		DX::ThrowIfFailed(
			CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&m_dxgiFactory))
		);

		DX::ThrowIfFailed(
			m_dxgiFactory->EnumAdapters1(0, m_adapter.ReleaseAndGetAddressOf())
		);

		static const D3D_FEATURE_LEVEL requestedFeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		UINT deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		DX::ThrowIfFailed(
			D3D11CreateDevice(m_adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, deviceFlags, requestedFeatureLevels, _countof(requestedFeatureLevels),
				D3D11_SDK_VERSION, m_d3dDevice.ReleaseAndGetAddressOf(), &m_featureLevel, m_d3dDeviceContext.ReleaseAndGetAddressOf())
		);

		// Initialize debug layer
		{
			ComPtr<ID3D11Debug> pDebug;
			m_d3dDevice.As(&pDebug);

			ComPtr<ID3D11InfoQueue> pInfoQueue;
			pDebug.As(&pInfoQueue);

#ifdef _DEBUG
			pInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			pInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif

			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			pInfoQueue->AddStorageFilterEntries(&filter);
		}

		//#ifdef _DEBUG
		//	ComPtr<IDXGIDebug1> debug;
		//	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		//	{
		//		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		//	}
		//#endif
	}

	void DeviceResources::CreateDeviceDependentResources()
	{
		// Initialize Direct2D factory
		{
#ifdef _DEBUG
			D2D1_FACTORY_OPTIONS options = {};
			options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

			DX::ThrowIfFailed(
				D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, m_d2dFactory.ReleaseAndGetAddressOf())
			);
#else
			DX::ThrowIfFailed(
				D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.ReleaseAndGetAddressOf())
			);
#endif
		}

		// Create Direct2D device and device context
		{
			ComPtr<IDXGIDevice> pDevice;
			m_d3dDevice.As(&pDevice);

			DX::ThrowIfFailed(
				m_d2dFactory->CreateDevice(pDevice.Get(), m_d2dDevice.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dDeviceContext.ReleaseAndGetAddressOf())
			);
		}

		// Initialize DirectWrite
		{
			DX::ThrowIfFailed(
				DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown**>(m_dWriteFactory.ReleaseAndGetAddressOf()))
			);
		}
	}

	void DeviceResources::CreateWindowSizeDependentResources()
	{
		if (!m_window)
		{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PC_APP)
			throw std::logic_error("Call SetWindow with a valid CoreWindow pointer");
#else
			throw std::logic_error("Call SetWindow with a valid Win32 window handle");
#endif
		}

		const auto width = m_outputSize.right - m_outputSize.left;
		const auto height = m_outputSize.bottom - m_outputSize.top;

		// Set viewport size
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = static_cast<float>(m_outputSize.right - m_outputSize.left);
		m_viewport.Height = static_cast<float>(m_outputSize.bottom - m_outputSize.top);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		// Resize SwapChain if already exists, otherwise create it
		if (m_swapChain)
		{
			// Release swap chain resources
			m_depthBuffer.Reset();
			m_backBuffer.Reset();
			m_renderTarget.Reset();

			DX::ThrowIfFailed(
				m_swapChain->ResizeBuffers(0, width, height, m_backBufferFormat, 0)
			);
		}
		else
		{
			DXGI_SWAP_CHAIN_DESC1 scd = {};
			scd.BufferCount = 1;
			scd.Width = static_cast<UINT>(m_viewport.Width);
			scd.Height = static_cast<UINT>(m_viewport.Height);
			scd.Format = m_backBufferFormat;
			scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scd.SampleDesc.Count = 1;
			scd.SampleDesc.Quality = 0;
			scd.Scaling = DXGI_SCALING_STRETCH;
			scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			scd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			scd.Flags = 0;

			ComPtr<IDXGIDevice4> pDevice;
			m_d3dDevice.As(&pDevice);

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PC_APP)
			scd.BufferCount = 2;
			scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

			DX::ThrowIfFailed(
				m_dxgiFactory->CreateSwapChainForCoreWindow(pDevice.Get(), m_window, &scd, nullptr, m_swapChain.ReleaseAndGetAddressOf())
			);
#else
			DX::ThrowIfFailed(
				m_dxgiFactory->CreateSwapChainForHwnd(dxgiDevice.Get(), m_window, &scd, nullptr, nullptr, m_swapChain.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));
#endif
		}

		// Get a reference to the back buffer
		DX::ThrowIfFailed(
			m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.ReleaseAndGetAddressOf()))
		);

		// Create render target view
		{
			CD3D11_RENDER_TARGET_VIEW_DESC1 rd(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);

			DX::ThrowIfFailed(
				GetD3DDevice()->CreateRenderTargetView1(m_backBuffer.Get(), &rd, m_renderTarget.ReleaseAndGetAddressOf())
			);
		}

		// Create depth stencil view
		if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_TEXTURE2D_DESC1 depthBufferDesc = {};
			depthBufferDesc.Width = static_cast<UINT>(m_viewport.Width);
			depthBufferDesc.Height = static_cast<UINT>(m_viewport.Height);
			depthBufferDesc.Format = m_depthBufferFormat;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			DX::ThrowIfFailed(GetD3DDevice()->CreateTexture2D1(&depthBufferDesc, nullptr, m_depthBuffer.ReleaseAndGetAddressOf()));

			D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Texture2D.MipSlice = 0;
			dsv.Format = m_depthBufferFormat;
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			DX::ThrowIfFailed(
				m_d3dDevice->CreateDepthStencilView(m_depthBuffer.Get(), &dsv, m_depthStencilView.ReleaseAndGetAddressOf())
			);
		}

		// Direct2D render target
		{
			wrl::ComPtr<IDXGISurface> pSurface;

			// Get a reference to the back buffer
			DX::ThrowIfFailed(
				m_swapChain->GetBuffer(0, IID_PPV_ARGS(pSurface.ReleaseAndGetAddressOf()))
			);

			const auto props = D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(m_backBufferFormat, D2D1_ALPHA_MODE_IGNORE)
			);

			// Create bitmap
			DX::ThrowIfFailed(
				m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
					pSurface.Get(),
					props,
					m_d2dBitmap.ReleaseAndGetAddressOf()
				)
			);

			// Set render target
			m_d2dDeviceContext->SetTarget(m_d2dBitmap.Get());
		}
	}
}
