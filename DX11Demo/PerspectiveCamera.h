#pragma once

#include "SceneObject.h"

namespace DX
{
	class PerspectiveCamera : public SceneObject
	{
	public:
		explicit PerspectiveCamera(const DirectX::SimpleMath::Viewport& viewport) noexcept;

		DirectX::SimpleMath::Viewport GetViewport() const noexcept { return m_viewport; }
		DirectX::SimpleMath::Matrix GetProjectionMatrix() noexcept;
		DirectX::SimpleMath::Matrix GetViewMatrix() noexcept;
		DirectX::SimpleMath::Matrix GetWorldMatrix() noexcept override;

		void SetViewport(const DirectX::SimpleMath::Viewport& viewport) noexcept;
		virtual void SetWorldPosition(const DirectX::SimpleMath::Vector3& position) noexcept override;
		virtual void SetWorldRotation(const DirectX::SimpleMath::Vector3& rotation) noexcept override;

		virtual void Translate(const DirectX::SimpleMath::Vector3& translation) noexcept override;
		virtual void Rotate(const DirectX::SimpleMath::Vector3& angles) noexcept;

	private:
		DirectX::SimpleMath::Viewport m_viewport;
		DirectX::SimpleMath::Matrix m_projectionMatrix;
		DirectX::SimpleMath::Matrix m_viewMatrix;
		bool m_dirtyProjectionMatrix;
		bool m_dirtyViewMatrix;
	};
}
