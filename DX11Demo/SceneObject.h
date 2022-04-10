#pragma once

class SceneObject
{
public:
	SceneObject() {}
	SceneObject(const DirectX::SimpleMath::Vector3& position) : worldPosition(position) {}
	virtual ~SceneObject() = default;

	DirectX::SimpleMath::Vector3 worldPosition;
	DirectX::SimpleMath::Vector3 worldRotation;

	DirectX::SimpleMath::Matrix GetWorldMatrix() const noexcept;
};

