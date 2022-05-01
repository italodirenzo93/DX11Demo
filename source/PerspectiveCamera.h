#pragma once

namespace DX
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera() noexcept;

		void Update() noexcept;

		const math::Vector3 GetRightVec() const noexcept { return m_Basis.Right(); }
		const math::Vector3 GetUpVec() const noexcept { return m_Basis.Up(); }
		const math::Vector3 GetFowardVec() const noexcept { return m_Basis.Forward(); }

		math::Matrix GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
		math::Matrix GetViewMatrix() const noexcept { return m_ViewMatrix; }
		math::Matrix GetViewProjectionMatrix() const noexcept { return m_ViewProjectionMatrix; }

		void SetEyeAtUp(const math::Vector3& eye, const math::Vector3& at, const math::Vector3& up);
		void SetLookDirection(const math::Vector3& forward, const math::Vector3& up);
		void SetPosition(const math::Vector3& position) noexcept;
		void SetRotation(const math::Quaternion& rotation) noexcept;

		void SetPerspectiveMatrix(float verticalFieldOfView, float aspectRatio, float nearZClip, float farZClip);

		void Translate(const math::Vector3& translation) noexcept;
		void Rotate(const math::Quaternion& rotation) noexcept;

	private:
		math::Matrix m_Basis;

		math::Vector3 m_Position;

		math::Matrix m_ProjectionMatrix;
		math::Matrix m_ViewMatrix;
		math::Matrix m_ViewProjectionMatrix;
		math::Matrix m_PreviousViewProjectionMatrix;
	};
}
