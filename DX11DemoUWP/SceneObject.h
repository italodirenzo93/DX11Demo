#pragma once

class SceneObject
{
public:
	virtual ~SceneObject() = default;

	DirectX::SimpleMath::Vector3 worldPosition;
	DirectX::SimpleMath::Vector3 worldRotation;

	DirectX::SimpleMath::Matrix GetWorldMatrix() const noexcept;
};

