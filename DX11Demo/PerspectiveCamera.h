#pragma once

namespace DX
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera() noexcept;

		void Update() noexcept;

		const DXMath::Vector3 GetRightVec() const noexcept { return m_Basis.Right(); }
		const DXMath::Vector3 GetUpVec() const noexcept { return m_Basis.Up(); }
		const DXMath::Vector3 GetFowardVec() const noexcept { return m_Basis.Forward(); }

		DXMath::Matrix GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
		DXMath::Matrix GetViewMatrix() const noexcept { return m_ViewMatrix; }
		DXMath::Matrix GetViewProjectionMatrix() const noexcept { return m_ViewProjectionMatrix; }

		void SetEyeAtUp(const DXMath::Vector3& eye, const DXMath::Vector3& at, const DXMath::Vector3& up);
		void SetLookDirection(const DXMath::Vector3& forward, const DXMath::Vector3& up);
		void SetPosition(const DXMath::Vector3& position) noexcept;
		void SetRotation(const DXMath::Quaternion& rotation) noexcept;

		void SetPerspectiveMatrix(float verticalFieldOfView, float aspectRatio, float nearZClip, float farZClip);

		void Translate(const DXMath::Vector3& translation) noexcept;
		void Rotate(const DXMath::Quaternion& rotation) noexcept;

	private:
		DXMath::Matrix m_Basis;

		DXMath::Vector3 m_Position;

		DXMath::Matrix m_ProjectionMatrix;
		DXMath::Matrix m_ViewMatrix;
		DXMath::Matrix m_ViewProjectionMatrix;
		DXMath::Matrix m_PreviousViewProjectionMatrix;
	};
}
