#pragma once

namespace DX
{
	class SceneObject
	{
	public:
		SceneObject() noexcept;
		SceneObject(const DirectX::SimpleMath::Vector3& position) noexcept;
		SceneObject(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& rotation) noexcept;
		virtual ~SceneObject() = default;

		DirectX::SimpleMath::Vector3 GetWorldPosition() const noexcept { return m_worldPosition; }
		DirectX::SimpleMath::Vector3 GetWorldRotation() const noexcept { return m_worldRotation; }
		virtual DirectX::SimpleMath::Matrix GetWorldMatrix() noexcept;

		virtual void SetWorldPosition(const DirectX::SimpleMath::Vector3& position) noexcept;
		virtual void SetWorldRotation(const DirectX::SimpleMath::Vector3& rotation) noexcept;

		virtual void Translate(const DirectX::SimpleMath::Vector3& translation) noexcept;
		virtual void Rotate(const DirectX::SimpleMath::Vector3& angles) noexcept;

	protected:
		DirectX::SimpleMath::Vector3 m_worldPosition;
		DirectX::SimpleMath::Vector3 m_worldRotation;

		DirectX::SimpleMath::Matrix m_worldMatrix;
		bool m_dirtyWorldMatrix;
	};
}
