#pragma once

#include "SceneObject.h"

namespace DX
{
	class PerspectiveCamera : public SceneObject
	{
	public:
		explicit PerspectiveCamera(const DirectX::SimpleMath::Viewport& viewport) noexcept;

		DirectX::SimpleMath::Matrix GetProjectionMatrix() noexcept;
		DirectX::SimpleMath::Matrix GetViewMatrix() noexcept;

		void SetViewport(const DirectX::SimpleMath::Viewport& viewport) noexcept;

	private:
		DirectX::SimpleMath::Viewport m_viewport;
		DirectX::SimpleMath::Matrix m_projectionMatrix;
		DirectX::SimpleMath::Matrix m_viewMatrix;
		bool m_dirtyProjectionMatrix;
		bool m_dirtyViewMatrix;
	};
}
