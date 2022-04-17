#include "pch.h"
#include "SceneObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DX
{
	SceneObject::SceneObject() noexcept
		:
		SceneObject(Vector3::Zero)
	{
	}

	SceneObject::SceneObject(const Vector3& position) noexcept
		:
		SceneObject(position, Quaternion::Identity)
	{
	}

	SceneObject::SceneObject(const Vector3& position, const Quaternion& rotation) noexcept
		:
		m_worldPosition(position),
		m_worldRotation(rotation),
		m_worldMatrix(Matrix::Identity),
		m_dirtyWorldMatrix(true)
	{
	}

	Matrix SceneObject::GetWorldMatrix() noexcept
	{
		if (m_dirtyWorldMatrix)
		{
			const auto translation = Matrix::CreateTranslation(m_worldPosition);
			const auto rotation = Matrix::CreateFromYawPitchRoll(m_worldRotation);
			
			m_worldMatrix = rotation * translation;

			m_dirtyWorldMatrix = false;
		}

		return m_worldMatrix;
	}

	void SceneObject::SetWorldPosition(const Vector3& position) noexcept
	{
		m_worldPosition = position;
		m_dirtyWorldMatrix = true;
	}

	void SceneObject::SetWorldRotation(const Vector3& rotation) noexcept
	{
		m_worldRotation = rotation;
		m_dirtyWorldMatrix = true;
	}

	void SceneObject::Translate(const Vector3& translation) noexcept
	{
		m_worldPosition = m_worldPosition + translation;
		m_dirtyWorldMatrix = true;
	}

	void SceneObject::Rotate(const Vector3& angles) noexcept
	{
		m_worldRotation = m_worldRotation + angles;
		m_dirtyWorldMatrix = true;
	}
}
