#include "pch.h"
#include "SceneObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Matrix SceneObject::GetWorldMatrix() const noexcept
{
	const auto position = Matrix::CreateTranslation(worldPosition);
	const auto rotation = Matrix::CreateFromYawPitchRoll(worldRotation);

	return position * rotation;
}
