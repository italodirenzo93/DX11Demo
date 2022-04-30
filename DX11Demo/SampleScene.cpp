#include "pch.h"
#include "SampleScene.h"

#include <DDSTextureLoader.h>

#include "ReadData.h"

using namespace DX;
using namespace DirectX;
using namespace DirectX::SimpleMath;

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

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PC_APP)

#include <winrt/Windows.Graphics.Display.Core.h>

void SampleScene::Initialize(const winrt::Windows::UI::Core::CoreWindow& window, int width, int height)
{
	m_window = winrt::get_unknown(window);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	OnWindowSizeChanged(width, height);
	
	auto pWindow = reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(winrt::get_abi(window));
	m_keyboard->SetWindow(pWindow);
	m_mouse->SetWindow(pWindow);

	// This is gross, move this into App.cpp somehow...
	auto di = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
	m_mouse->SetDpi(di.LogicalDpi());

	//m_mouse->SetVisible(false);
	//m_mouse->SetMode(Mouse::MODE_RELATIVE);
}

#else

void SampleScene::Initialize(HWND hWnd, int width, int height)
{
	m_window = hWnd;

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	OnWindowSizeChanged(width, height);
}

#endif

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
	const auto device = m_deviceResources->GetD3DDevice();

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

		DX::ThrowIfFailed(
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
		DX::ThrowIfFailed(
			CreateDDSTextureFromFile(
				device,
				m_deviceResources->GetD3DDeviceContext(),
				L"Assets/braynzar.dds",
				nullptr,
				m_cubeTexture.ReleaseAndGetAddressOf()
			)
		);

		m_effect->SetTexture(m_cubeTexture.Get());
	}

	DX::ThrowIfFailed(
		m_deviceResources->GetDWriteFactory()->CreateTextFormat(
			L"Arial",
			nullptr,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			24.0f,
			L"en-us",
			m_textFormat.ReleaseAndGetAddressOf()
		)
	);
}

void SampleScene::CreateWindowSizeDependentResources()
{
	m_camera = std::make_unique<PerspectiveCamera>();

	const auto viewport = m_deviceResources->GetViewport();
	m_camera->SetPerspectiveMatrix(XM_PIDIV4, viewport.Width / viewport.Height, 1.0f, 1000.0f);

	m_camera->SetPosition(Vector3(0.0f, 0.0f, -3.0f));

	m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_brush.ReleaseAndGetAddressOf());
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

	if (keyboard.Z)
	{
		//m_camera->SetRotation(Quaternion::CreateFromYawPitchRoll(Vector3(0.0f, -0.5f, 0.0f)));
		m_camera->Rotate(Quaternion::CreateFromYawPitchRoll(-Vector3::UnitY * deltaTime));
	}
	if (keyboard.C)
	{
		//m_camera->SetRotation(Quaternion::CreateFromYawPitchRoll(Vector3(0.0f, 0.5f, 0.0f)));
		m_camera->Rotate(Quaternion::CreateFromYawPitchRoll(Vector3::UnitY * deltaTime));
	}

	// mouse look
	//if (m_mouse->IsConnected())
	//{
	//	const auto mouse = m_mouse->GetState();

	//	m_camera->Rotate(Vector3(float(mouse.x), float(-mouse.y), 0.0f) * deltaTime);
	//}

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
	m_camera->Update();

	const auto ctx = m_deviceResources->GetD3DDeviceContext();

	// Clear render targets
	ID3D11RenderTargetView* rtv = m_deviceResources->GetRenderTargetView();
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

	// Draw 2D overlay
	{
		const auto size = m_deviceResources->GetOutputSize();
		const auto d2dContext = m_deviceResources->GetD2DDeviceContext();
		

		d2dContext->BeginDraw();

		static const std::wstring text = L"Hello World!";

		d2dContext->DrawText(
			text.c_str(),
			static_cast<UINT32>(text.length()),
			m_textFormat.Get(),
			D2D1::RectF(
				float(size.left + 5),
				float(size.top + 5),
				float(size.right - 5),
				float(size.bottom - 5)
			),
			m_brush.Get()
		);

		d2dContext->EndDraw();
	}

	// Show the back buffer
	m_deviceResources->GetSwapChain()->Present(1, 0);
}
