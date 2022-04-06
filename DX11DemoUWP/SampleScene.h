#pragma once

#include "StepTimer.h"
#include "DeviceResources.h"
#include "CubeObject.h"
#include "MyEffect.h"

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

	std::unique_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<MyEffect> m_effect;

	std::vector<CubeObject> m_objects;

	std::unique_ptr<DirectX::CommonStates> m_commonStates;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	std::unique_ptr<DirectX::GamePad> m_gamepad;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeTexture;

	::IUnknown* m_window;
};

