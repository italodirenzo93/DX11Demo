#pragma once

class SceneObject
{
public:
	SceneObject() noexcept;
	SceneObject(const DirectX::SimpleMath::Vector3& position) noexcept;
	SceneObject(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation) noexcept;
	virtual ~SceneObject() = default;

	DirectX::SimpleMath::Vector3 GetWorldPosition() const noexcept { return m_worldPosition; }
	DirectX::SimpleMath::Quaternion GetWorldRotation() const noexcept { return m_worldRotation; }
	DirectX::SimpleMath::Matrix GetWorldMatrix() noexcept;

	void SetWorldPosition(const DirectX::SimpleMath::Vector3& position) noexcept;
	void SetWorldRotation(const DirectX::SimpleMath::Quaternion& rotation) noexcept;

protected:
	DirectX::SimpleMath::Vector3 m_worldPosition;
	DirectX::SimpleMath::Quaternion m_worldRotation;

	DirectX::SimpleMath::Matrix m_worldMatrix;

private:
	bool m_dirtyWorldMatrix;
};

