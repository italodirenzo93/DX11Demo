#pragma once

namespace DX
{
	class BaseCamera
	{
	protected:
		using Vec3 = DirectX::SimpleMath::Vector3;
		using Mat4 = DirectX::SimpleMath::Matrix;
		using Quat = DirectX::SimpleMath::Quaternion;

	public:
		// Call this function once per frame and after you've changed any state.  This
		// regenerates all matrices.  Calling it more or less than once per frame will break
		// temporal effects and cause unpredictable results.
		void Update();

		void SetEyeAtUp(const Vec3& eye, const Vec3& at, const Vec3& up);
		void SetLookDirection(const Vec3& forward, const Vec3& up);
		void SetRotation(const Quat& basisRotation);
		void SetPosition(const Vec3& worldPosition);

		//const DXMath::Quaternion GetRotation() const;
		const Vec3 GetRightVec() const noexcept { return Vec3::UnitX; }
		const Vec3 GetUpVec() const noexcept { return Vec3::UnitY; }
		const Vec3 GetForwardVec() const noexcept { return Vec3::UnitZ; }
		//const Vec3 GetPosition() const;

		const Mat4& GetViewMatrix() const noexcept { return m_ViewMatrix; }
		const Mat4& GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
		const Mat4& GetViewProjectionMatrix() const noexcept { return m_ViewProjectionMatrix; }
		//const Mat4& GetReprojectionMatrix() const;

	protected:
		BaseCamera() : m_Basis(Mat4::Identity) {}

		void SetProjectionMatrix(const Mat4& matrix) { m_ProjectionMatrix = matrix; }

		// Redundant data cached for faster lookups
		Mat4 m_Basis;

		Mat4 m_ViewMatrix;
		Mat4 m_ProjectionMatrix;
		Mat4 m_ViewProjectionMatrix;
		Mat4 m_PreviousViewProjectionMatrix;
		//Mat4 m_ReprojectMatrix;
	};

	class Camera : public BaseCamera
	{
	public:
		Camera();

		void SetPerspectiveMatrix(float verticalFovRadians, float aspectHeightOverWidth, float nearZClip, float farZClip);
		void SetFieldOfView(float verticalFovRadians);
		void SetAspectRatio(float heightOverWidth);
		void SetZRange(float nearZ, float farZ);
		void ReverseZ(bool enable);

		float GetFieldOfView() const noexcept;
		float GetNearClip() const noexcept;
		float GetFarClip() const noexcept;
		float GetClearDepth() const noexcept;

	private:
		void UpdateProjectionMatrix();

		float m_VerticalFieldOfView;	// FOV in radians
		float m_AspectRatio;
		float m_NearClip;
		float m_FarClip;
		bool m_ReverseZ;	// Invert near and far clip distances so that Z=1 at the near plane
		bool m_InfiniteZ;	// Far plane is infinite
	};

	inline void BaseCamera::SetEyeAtUp(const BaseCamera::Vec3& eye, const BaseCamera::Vec3& at, const BaseCamera::Vec3& up)
	{
		SetLookDirection(at - eye, up);
		SetPosition(eye);
	}

	inline void BaseCamera::SetPosition(const BaseCamera::Vec3& worldPosition)
	{

	}

	inline void BaseCamera::SetRotation(const BaseCamera::Quat& basisRotation)
	{
		m_Basis = Mat4::CreateFromQuaternion(basisRotation);
	}

	inline Camera::Camera() : m_ReverseZ(true), m_InfiniteZ(false)
	{
		SetPerspectiveMatrix(DirectX::XM_PI / 4.0f, 9.0f / 16.0f, 1.0f, 1000.0f);
	}

	inline void Camera::SetPerspectiveMatrix(float verticalFovRadians, float aspectHeightOverWidth, float nearZClip, float farZClip)
	{
		m_VerticalFieldOfView = verticalFovRadians;
		m_AspectRatio = aspectHeightOverWidth;
		m_NearClip = nearZClip;
		m_FarClip = farZClip;

		UpdateProjectionMatrix();

		m_PreviousViewProjectionMatrix = m_ViewProjectionMatrix;
	}
}

