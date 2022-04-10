#include "pch.h"
#include "SampleScene.h"

#include <DDSTextureLoader.h>

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
	const auto device = m_deviceResources->GetDevice();

	// Init common states
	m_commonStates = std::make_unique<CommonStates>(device);
	m_effect = std::make_unique<MyEffect>(device);

	m_objects.push_back(CubeObject(device, Vector3(1.0f, -1.0f, 1.5f)));
	m_objects.push_back(CubeObject(device, Vector3(-1.0f, 1.0f, 1.5f)));
	m_objects.push_back(CubeObject(device, Vector3(0.0f, 0.0f, 2.5f)));

	// Vertex program
	{
		const void* bytecodeData;
		size_t bytecodeLength;
		m_effect->GetVertexShaderBytecode(&bytecodeData, &bytecodeLength);

		ThrowIfFailed(
			device->CreateInputLayout(
				MyEffect::InputLayout,
				MyEffect::InputLayoutCount,
				bytecodeData,
				bytecodeLength,
				m_inputLayout.ReleaseAndGetAddressOf()
			)
		);
	}

	// Load texture for cube
	{
		ThrowIfFailed(
			CreateDDSTextureFromFile(
				device,
				m_deviceResources->GetDeviceContext(),
				L"Assets/braynzar.dds",
				nullptr,
				m_cubeTexture.ReleaseAndGetAddressOf()
			)
		);

		m_effect->SetTexture(m_cubeTexture.Get());
	}
}

void SampleScene::CreateWindowSizeDependentResources()
{
	const auto vp = m_deviceResources->GetViewport();

	m_effect->SetProjection( XMMatrixPerspectiveFovLH(XM_PI / 4.f, vp.AspectRatio(), 0.1f, 10.f) );
	m_effect->SetView( XMMatrixLookAtLH(Vector3(0.0f, 1.0f, -3.0f), Vector3::Zero, Vector3::UnitY) );
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

	// Rotate each cube
	for (auto& obj : m_objects)
	{
		obj.worldRotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY, elapsed).ToEuler();
	}
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

	// Draw each cube
	for (auto& obj : m_objects)
	{
		const auto vertexBuffers = obj.GetVertexBuffer();
		const auto strides = obj.GetStride();
		const UINT offsets = 0;
		ctx->IASetVertexBuffers(0, 1, &vertexBuffers, &strides, &offsets);
		ctx->IASetIndexBuffer(obj.GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

		m_effect->SetWorld(obj.GetWorldMatrix());
		m_effect->Apply(ctx);

		// Draw things
		ctx->DrawIndexed(obj.GetElementCount(), 0, 0);
	}

	// Show the back buffer
	m_deviceResources->GetSwapChain()->Present(1, 0);
}
