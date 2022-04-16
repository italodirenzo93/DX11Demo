#include "pch.h"
#include "PerspectiveCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DX
{
	PerspectiveCamera::PerspectiveCamera(const Viewport& viewport) noexcept
		:
		m_viewport(viewport),
		m_projectionMatrix(Matrix::Identity),
		m_dirtyProjectionMatrix(true),
		m_dirtyViewMatrix(true)
	{
	}

	DirectX::SimpleMath::Matrix PerspectiveCamera::GetProjectionMatrix() noexcept
	{
		if (m_dirtyProjectionMatrix)
		{
			m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, m_viewport.AspectRatio(), 0.1f, 10.0f);

			m_dirtyProjectionMatrix = false;
		}
		
		return m_projectionMatrix;
	}

	DirectX::SimpleMath::Matrix PerspectiveCamera::GetViewMatrix() noexcept
	{
		if (m_dirtyViewMatrix)
		{
			const auto matWorld = GetWorldMatrix();

			m_viewMatrix = XMMatrixLookAtLH(matWorld.Translation(), matWorld.Forward(), Vector3::Up);

			m_dirtyViewMatrix = false;
		}

		return m_viewMatrix;
	}

	void PerspectiveCamera::SetViewport(const Viewport& viewport) noexcept
	{
		m_viewport = viewport;
	}
}
