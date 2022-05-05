#pragma once

namespace DX
{
	class PerspectiveCamera
	{
	public:
		using VectorType = DirectX::SimpleMath::Vector3;
		using MatrixType = DirectX::SimpleMath::Matrix;
		using QuaternionType = DirectX::SimpleMath::Quaternion;

		PerspectiveCamera() noexcept;

		void Update() noexcept;

		const VectorType GetRightVec() const noexcept { return m_Basis.Right(); }
		const VectorType GetUpVec() const noexcept { return m_Basis.Up(); }
		const VectorType GetFowardVec() const noexcept { return m_Basis.Forward(); }

		MatrixType GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
		MatrixType GetViewMatrix() const noexcept { return m_ViewMatrix; }
		MatrixType GetViewProjectionMatrix() const noexcept { return m_ViewProjectionMatrix; }

		void SetEyeAtUp(const VectorType& eye, const VectorType& at, const VectorType& up);
		void SetLookDirection(const VectorType& forward, const VectorType& up);
		void SetPosition(const VectorType& position) noexcept;
		void SetRotation(const QuaternionType& rotation) noexcept;

		void SetPerspectiveMatrix(float verticalFieldOfView, float aspectRatio, float nearZClip, float farZClip);

		void Translate(const VectorType& translation) noexcept;
		void Rotate(const QuaternionType& rotation) noexcept;

	private:
		MatrixType m_Basis;

		VectorType m_Position;

		MatrixType m_ProjectionMatrix;
		MatrixType m_ViewMatrix;
		MatrixType m_ViewProjectionMatrix;
		MatrixType m_PreviousViewProjectionMatrix;
	};
}
