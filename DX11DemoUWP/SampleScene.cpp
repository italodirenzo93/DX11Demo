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

struct VertexType
{
	Vector3 position;
	Vector2 tex;
};

static UINT stride = sizeof(VertexType);
static UINT offset = 0;

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

	// Create Vertex Buffer
	{
		GeometricPrimitive::VertexCollection vertices;
		GeometricPrimitive::IndexCollection indices;
		GeometricPrimitive::CreateCube(vertices, indices);

		std::vector<VertexType> newVerts;
		newVerts.reserve(vertices.size());
		for (const auto& v : vertices)
		{
			VertexType vert;
			vert.position = v.position;
			vert.tex = v.textureCoordinate;
			newVerts.emplace_back(vert);
		}

		ThrowIfFailed(
			CreateStaticBuffer(m_deviceResources->GetDevice(), newVerts, D3D11_BIND_VERTEX_BUFFER, m_vertexBuffer.ReleaseAndGetAddressOf())
		);

		ThrowIfFailed(
			CreateStaticBuffer(m_deviceResources->GetDevice(), indices, D3D11_BIND_INDEX_BUFFER, m_indexBuffer.ReleaseAndGetAddressOf())
		);

		m_indexCount = static_cast<UINT>(indices.size());
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

	m_projection = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, vp.AspectRatio(), 0.1f, 10.f);
	m_view = Matrix::CreateLookAt(Vector3(0.0f, 2.0f, -3.0f), Vector3::Zero, Vector3::UnitY);
	m_world = Matrix::Identity;
}

void SampleScene::Update(const StepTimer& timer)
{
	if (m_keyboard->GetState().Escape || m_gamepad->GetState(0).IsBPressed())
	{
		ExitApplication();
	}

	const float elapsed = float(timer.GetTotalSeconds());

	m_world = Matrix::CreateRotationY(elapsed);

	VS_CONSTANT_BUFFER cb = {};
	cb.matWorldViewProj = m_world * m_view * m_projection;
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

	const auto vertexBuffers = m_vertexBuffer.Get();
	ctx->IASetVertexBuffers(0, 1, &vertexBuffers, &stride, &offset);
	ctx->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

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
	ctx->DrawIndexed(m_indexCount, 0, 0);

	// Show the back buffer
	m_deviceResources->GetSwapChain()->Present(1, 0);
}
