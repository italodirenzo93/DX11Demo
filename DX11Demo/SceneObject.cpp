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

	SceneObject::SceneObject(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation) noexcept
		:
		m_worldPosition(position),
		m_worldRotation(rotation),
		m_worldMatrix(Matrix::Identity),
		m_dirtyWorldMatrix(true)
	{
	}

	DirectX::SimpleMath::Matrix SceneObject::GetWorldMatrix() noexcept
	{
		if (m_dirtyWorldMatrix)
		{
			const auto translation = Matrix::CreateWorld(m_worldPosition, Vector3::Forward, Vector3::Up);
			//const auto translation = Matrix::CreateWorld(m_worldPosition, m_worldRotation., Vector3::Up);
			const auto rotation = Matrix::CreateFromQuaternion(m_worldRotation);

			m_worldMatrix = rotation * translation;

			m_dirtyWorldMatrix = false;
		}

		return m_worldMatrix;
	}

	void SceneObject::SetWorldPosition(const DirectX::SimpleMath::Vector3& position) noexcept
	{
		m_worldPosition = position;
		m_dirtyWorldMatrix = true;
	}

	void SceneObject::SetWorldRotation(const DirectX::SimpleMath::Quaternion& rotation) noexcept
	{
		m_worldRotation = rotation;
		m_dirtyWorldMatrix = true;
	}
}
