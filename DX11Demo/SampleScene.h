#pragma once

#include "StepTimer.h"
#include "DeviceResources.h"
#include "CubeObject.h"
#include "MyEffect.h"
#include "PerspectiveCamera.h"

class SampleScene
{
public:
	SampleScene();
	~SampleScene();

	void Initialize(const winrt::Windows::UI::Core::CoreWindow& window, int width, int height);
	void OnWindowSizeChanged(int width, int height);
	void Tick();

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	void Update(const DX::StepTimer& timer);
	void Render();

	DX::StepTimer m_timer;
	float m_lastFrameTime;

	std::unique_ptr<DX::DeviceResources> m_deviceResources;
	std::unique_ptr<DX::MyEffect> m_effect;
	std::unique_ptr<DX::PerspectiveCamera> m_camera;

	std::vector<DX::CubeObject> m_objects;

	std::unique_ptr<DirectX::CommonStates> m_commonStates;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	std::unique_ptr<DirectX::GamePad> m_gamepad;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeTexture;

	::IUnknown* m_window;
};

