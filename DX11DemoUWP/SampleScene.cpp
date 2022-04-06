#include "pch.h"
#include "SampleScene.h"

#include <DDSTextureLoader.h>
#include <GeometricPrimitive.h>

#include "ReadData.h"

using namespace DX;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

extern void ExitApplication() noexcept;

SampleScene::SampleScene()
{
	m_deviceResources = std::make_unique<DeviceResources>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_gamepad = std::make_unique<GamePad>();
	m_window = nullptr;
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize(const winrt::Windows::UI::Core::CoreWindow& window, int width, int height)
{
	m_window = winrt::get_unknown(window);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	OnWindowSizeChanged(width, height);

	m_keyboard->SetWindow(window);
	m_mouse->SetWindow(window);
}

void SampleScene::OnWindowSizeChanged(int width, int height)
{
	m_deviceResources->SetWindow(m_window, width, height);
	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();
}

void SampleScene::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}

void SampleScene::GetDefaultSize(int& width, int& height) const noexcept
{
	width = 1280;
	height = 720;
}

void SampleScene::CreateDeviceDependentResources()
{
	// Init common states
	m_commonStates = std::make_unique<CommonStates>(m_deviceResources->GetDevice());
	m_cubeObject = std::make_unique<CubeObject>(m_deviceResources->GetDevice());

	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	// Vertex program
	{
		auto shaderBlob = ReadData(L"VertexShader.cso");

		ThrowIfFailed(
			m_deviceResources->GetDevice()->CreateVertexShader(
				shaderBlob.data(),
				shaderBlob.size(),
				nullptr,
				m_vertexShader.ReleaseAndGetAddressOf()
			)
		);


		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ThrowIfFailed(
			m_deviceResources->GetDevice()->CreateInputLayout(
				layout,
				ARRAYSIZE(layout),
				shaderBlob.data(),
				shaderBlob.size(),
				m_inputLayout.ReleaseAndGetAddressOf()
			)
		);
	}

	// Pixel program
	{
		auto shaderBlob = ReadData(L"PixelShader.cso");

		ThrowIfFailed(
			m_deviceResources->GetDevice()->CreatePixelShader(
				shaderBlob.data(),
				shaderBlob.size(),
				nullptr,
				m_pixelShader.ReleaseAndGetAddressOf()
			)
		);
	}

	// Create constant buffer
	{
		m_constantBuffer.Create(m_deviceResources->GetDevice());
	}

	// Load texture for cube
	{
		ThrowIfFailed(
			CreateDDSTextureFromFile(
				m_deviceResources->GetDevice(),
				m_deviceResources->GetDeviceContext(),
				L"Assets/braynzar.dds",
				nullptr,
				m_cubeTexture.ReleaseAndGetAddressOf()
			)
		);
	}
}

void SampleScene::CreateWindowSizeDependentResources()
{
	const auto vp = m_deviceResources->GetViewport();

	m_projection = XMMatrixPerspectiveFovLH(XM_PI / 4.f, vp.AspectRatio(), 0.1f, 10.f);
	m_view = XMMatrixLookAtLH(Vector3(0.0f, 1.0f, -3.0f), Vector3::Zero, Vector3::UnitY);
}

void SampleScene::Update(const StepTimer& timer)
{
	const auto keyboard = m_keyboard->GetState();
	const auto gamepad = m_gamepad->GetState(0);

	if (keyboard.Escape || gamepad.IsBPressed())
	{
		ExitApplication();
	}
	
	const float elapsed = float(timer.GetTotalSeconds());

	m_cubeObject->worldRotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY, elapsed).ToEuler();

	VS_CONSTANT_BUFFER cb = {};
	cb.matWorldViewProj = m_cubeObject->GetWorldMatrix() * m_view * m_projection;
	m_constantBuffer.SetData(m_deviceResources->GetDeviceContext(), cb);
}

void SampleScene::Render()
{
	const auto ctx = m_deviceResources->GetDeviceContext();

	// Clear render targets
	ID3D11RenderTargetView* rtv = m_deviceResources->GetRenderTarget();
	ctx->ClearRenderTargetView(rtv, Colors::CornflowerBlue);
	ctx->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Rasterizer state
	ctx->RSSetViewports(1, m_deviceResources->GetViewport().Get11());
	ctx->RSSetState(m_commonStates->CullNone());

	// Output-merger
	ctx->OMSetBlendState(m_commonStates->Opaque(), Colors::White, 0xFFFFFFFF);
	ctx->OMSetDepthStencilState(m_commonStates->DepthDefault(), 0);
	ctx->OMSetRenderTargets(1, &rtv, m_deviceResources->GetDepthStencilView());

	// Input assembler
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->IASetInputLayout(m_inputLayout.Get());

	const auto vertexBuffers = m_cubeObject->GetVertexBuffer();
	const auto strides = m_cubeObject->GetStride();
	const UINT offsets = 0;
	ctx->IASetVertexBuffers(0, 1, &vertexBuffers, &strides, &offsets);
	ctx->IASetIndexBuffer(m_cubeObject->GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

	// Vertex shader
	const auto constantBuffers = m_constantBuffer.GetBuffer();
	ctx->VSSetConstantBuffers(0, 1, &constantBuffers);
	ctx->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Pixel shader
	const auto samplerState = m_commonStates->PointClamp();
	ctx->PSSetSamplers(0, 1, &samplerState);
	ctx->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	const auto psResources = m_cubeTexture.Get();
	ctx->PSSetShaderResources(0, 1, &psResources);

	// Draw things
	ctx->DrawIndexed(m_cubeObject->GetElementCount(), 0, 0);

	// Show the back buffer
	m_deviceResources->GetSwapChain()->Present(1, 0);
}
