#include "pch.h"
#include "SampleScene.h"

#include <DDSTextureLoader.h>

#include "ReadData.h"

using namespace DX;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

extern void ExitApplication() noexcept;

static constexpr float CameraSpeed = 7.0f;

SampleScene::SampleScene()
	:
	m_lastFrameTime(0.0f)
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

	// This is gross, move this into App.cpp somehow...
	auto di = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
	m_mouse->SetDpi(di.LogicalDpi());

	m_mouse->SetVisible(false);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);
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

	m_spriteBatch = std::make_unique<SpriteBatch>(m_deviceResources->GetDeviceContext());
	m_spriteFont = std::make_unique<SpriteFont>(device, L"Assets/consolas.spritefont");
}

void SampleScene::CreateWindowSizeDependentResources()
{
	m_camera = std::make_unique<PerspectiveCamera>(Viewport(m_deviceResources->GetViewport()));
	m_camera->SetWorldPosition(Vector3(0.0f, 0.0f, -3.0f));
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
	const float deltaTime = elapsed - m_lastFrameTime;
	
	// Camera controls (keyboard)
	if (keyboard.A)
	{
		m_camera->Translate(-Vector3::UnitX * CameraSpeed * deltaTime);
	}
	if (keyboard.D)
	{
		m_camera->Translate(Vector3::UnitX * CameraSpeed * deltaTime);
	}
	if (keyboard.W)
	{
		m_camera->Translate(Vector3::UnitZ * CameraSpeed * deltaTime);
	}
	if (keyboard.S)
	{
		m_camera->Translate(-Vector3::UnitZ * CameraSpeed * deltaTime);
	}
	if (keyboard.Q)
	{
		m_camera->Translate(Vector3::UnitY * CameraSpeed * deltaTime);
	}
	if (keyboard.E)
	{
		m_camera->Translate(-Vector3::UnitY * CameraSpeed * deltaTime);
	}

	// mouse look
	if (m_mouse->IsConnected())
	{
		const auto mouse = m_mouse->GetState();

		m_camera->Rotate(Vector3(float(mouse.x), float(-mouse.y), 0.0f) * deltaTime);
	}

	// Camera controls (gamepad)
	if (gamepad.IsConnected())
	{
		m_camera->Translate(
			Vector3(
				gamepad.thumbSticks.leftX * CameraSpeed * deltaTime,
				0.0f,
				gamepad.thumbSticks.leftY * CameraSpeed * deltaTime
			)
		);
	}

	// Rotate each cube
	for (auto& obj : m_objects)
	{
		obj.Rotate(Vector3(0.0f, deltaTime, 0.0f));
	}

	m_lastFrameTime = elapsed;
}

void SampleScene::Render()
{
	const auto ctx = m_deviceResources->GetDeviceContext();

	// Clear render targets
	ID3D11RenderTargetView* rtv = m_deviceResources->GetRenderTarget();
	ctx->ClearRenderTargetView(rtv, Colors::CornflowerBlue);
	ctx->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Rasterizer state
	const auto viewport = m_deviceResources->GetViewport();
	ctx->RSSetViewports(1, &viewport);
	ctx->RSSetState(m_commonStates->CullClockwise());

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

		m_effect->SetMatrices(obj.GetWorldMatrix(), m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix());
		m_effect->Apply(ctx);

		// Draw things
		ctx->DrawIndexed(obj.GetElementCount(), 0, 0);
	}
	
	// Draw UI text
	m_spriteBatch->Begin();
	
	sprintf_s(fpsText, "%d FPS", m_timer.GetFramesPerSecond());
	//sprintf_s(fpsText, "X: %d, Y: %d", int(m_mousePos.x), int(m_mousePos.y));

	m_spriteFont->DrawString(m_spriteBatch.get(), fpsText, Vector3::Zero);

	m_spriteBatch->End();

	// Show the back buffer
	m_deviceResources->GetSwapChain()->Present(1, 0);
}
