#include "pch.h"
#include "Camera.h"

namespace DX
{
	void BaseCamera::Update()
	{
		m_PreviousViewProjectionMatrix = m_ViewProjectionMatrix;

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		// reproject

		// frustum
	}

	void BaseCamera::SetLookDirection(const Vec3& forward, const Vec3& up)
	{

	}
}
