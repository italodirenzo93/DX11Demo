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

	Matrix PerspectiveCamera::GetProjectionMatrix() noexcept
	{
		if (m_dirtyProjectionMatrix)
		{
			m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, m_viewport.AspectRatio(), 0.1f, 1000.0f);

			m_dirtyProjectionMatrix = false;
		}
		
		return m_projectionMatrix;
	}

	Matrix PerspectiveCamera::GetViewMatrix() noexcept
	{
		if (m_dirtyViewMatrix)
		{
			m_viewMatrix = XMMatrixLookToLH(m_worldPosition, Vector3::UnitZ + m_worldRotation, Vector3::UnitY);

			m_dirtyViewMatrix = false;
		}

		return m_viewMatrix;
	}

	Matrix PerspectiveCamera::GetWorldMatrix() noexcept
	{
		// World matrix data (position, rotation) are handled by the camera's view matrix
		return Matrix::Identity;
	}

	void PerspectiveCamera::SetViewport(const Viewport& viewport) noexcept
	{
		m_viewport = viewport;
	}

	void PerspectiveCamera::SetWorldPosition(const Vector3& position) noexcept
	{
		SceneObject::SetWorldPosition(position);
		m_dirtyViewMatrix = true;
	}

	void PerspectiveCamera::SetWorldRotation(const Quaternion& rotation) noexcept
	{
		SceneObject::SetWorldRotation(rotation);
		m_dirtyViewMatrix = true;
	}

	void PerspectiveCamera::Translate(const Vector3& translation) noexcept
	{
		SceneObject::Translate(translation);
		m_dirtyViewMatrix = true;
	}
}
