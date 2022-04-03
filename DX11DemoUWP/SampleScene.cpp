#include "pch.h"
#include "SampleScene.h"

using namespace DX;
using namespace DirectX;

SampleScene::SampleScene()
{
	m_deviceResources = std::make_unique<DeviceResources>();
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize(::IUnknown* window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();
}

void SampleScene::OnWindowSizeChanged(float width, float height)
{
}

void SampleScene::Tick()
{
	Update(m_timer);
	Render();
}

void SampleScene::CreateDeviceDependentResources()
{
}

void SampleScene::CreateWindowSizeDependentResources()
{
}

void SampleScene::Update(const DX::StepTimer& timer)
{
}

void SampleScene::Render()
{
	const auto ctx = m_deviceResources->GetDeviceContext();
	ID3D11RenderTargetView* rtv = m_deviceResources->GetRenderTarget();

	ctx->ClearRenderTargetView(rtv, Colors::CornflowerBlue);

	ctx->OMSetRenderTargets(1, &rtv, nullptr);

	m_deviceResources->GetSwapChain()->Present(1, 0);
}
