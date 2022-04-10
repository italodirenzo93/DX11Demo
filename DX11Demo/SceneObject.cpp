#include "pch.h"
#include "SceneObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Matrix SceneObject::GetWorldMatrix() const noexcept
{
	const auto position = Matrix::CreateWorld(worldPosition, Vector3::UnitZ, Vector3::UnitY);
	const auto rotation = Matrix::CreateFromYawPitchRoll(worldRotation);

	return rotation * position;
}
