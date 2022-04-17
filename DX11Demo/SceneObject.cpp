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
			//const auto translation = Matrix::CreateWorld(m_worldPosition, m_worldRotation., Vector3::Up);
			const auto rotation = Matrix::CreateFromQuaternion(m_worldRotation);
			
			m_worldMatrix = rotation * translation;
			//m_worldMatrix = Matrix::CreateWorld(m_worldPosition, rotation.Forward(), rotation.Up());

			m_dirtyWorldMatrix = false;
		}

		return m_worldMatrix;
	}

	void SceneObject::SetWorldPosition(const Vector3& position) noexcept
	{
		m_worldPosition = position;
		m_dirtyWorldMatrix = true;
	}

	void SceneObject::SetWorldRotation(const Quaternion& rotation) noexcept
	{
		m_worldRotation = rotation;
		m_dirtyWorldMatrix = true;
	}

	void SceneObject::Translate(const Vector3& translation) noexcept
	{
		m_worldPosition = m_worldPosition + translation;
		m_dirtyWorldMatrix = true;
	}
}
