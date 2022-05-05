#include "pch.h"
#include "PerspectiveCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

static const Vector3 CameraForwardVector = Vector3(0.0f, 0.0f, 1.0f);
static const Vector3 CameraUpVector = Vector3(0.0f, 1.0f, 0.0f);

namespace DX
{
	PerspectiveCamera::PerspectiveCamera() noexcept
		:
		m_Basis(Matrix::Identity),
		m_ProjectionMatrix(Matrix::Identity),
		m_ViewMatrix(Matrix::Identity),
		m_ViewProjectionMatrix(Matrix::Identity),
		m_PreviousViewProjectionMatrix(Matrix::Identity)
	{
	}

	void PerspectiveCamera::Update() noexcept
	{
		m_PreviousViewProjectionMatrix = m_ViewProjectionMatrix;

		auto camTarget = Vector3::Transform(CameraForwardVector, m_Basis);
		camTarget += m_Position;

		auto camUp = Vector3::TransformNormal(CameraUpVector, m_Basis);

		m_ViewMatrix = XMMatrixLookAtLH(m_Position, camTarget, camUp);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::SetEyeAtUp(const Vector3& eye, const Vector3& at, const Vector3& up)
	{
		SetLookDirection(at - eye, up);
		SetPosition(eye);
	}

	void PerspectiveCamera::SetLookDirection(const Vector3& forward, const Vector3& up)
	{
		
	}

	void PerspectiveCamera::SetPosition(const Vector3& position) noexcept
	{
		m_Position = position;
	}

	void PerspectiveCamera::SetRotation(const Quaternion& rotation) noexcept
	{
		m_Basis = Matrix::CreateFromQuaternion(rotation);
	}

	void PerspectiveCamera::SetPerspectiveMatrix(float verticalFieldOfView, float aspectRatio, float nearZClip, float farZClip)
	{
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(
			verticalFieldOfView,
			aspectRatio,
			nearZClip,
			farZClip
		);
	}

	void PerspectiveCamera::Translate(const Vector3& translation) noexcept
	{
		SetPosition(m_Position + translation);
	}

	void PerspectiveCamera::Rotate(const Quaternion& rotation) noexcept
	{
		const auto rot = Quaternion::CreateFromRotationMatrix(m_Basis) + rotation;
		SetRotation(rot);
	}
}
