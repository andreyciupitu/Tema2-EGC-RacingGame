#include "RacingGame/SpatialObject.h"

#include <Core/Engine.h>

glm::mat4 SpatialObject::GetModelMatrix()
{
	//glm::mat4 modelMatrix(1);
	modelMatrix *= Transform3D::Translate(transform);
	modelMatrix *= Transform3D::RotateOX(rotation.x);
	modelMatrix *= Transform3D::RotateOY(rotation.y);
	modelMatrix *= Transform3D::RotateOZ(rotation.z);
	modelMatrix *= Transform3D::Scale(scale);

	return modelMatrix;
}

glm::vec3 SpatialObject::GetTransform()
{
	return transform;
}

void SpatialObject::SetTransform(const glm::vec3 &position)
{
	transform = position;
}

glm::vec3 SpatialObject::GetRotation()
{
	return rotation;
}

void SpatialObject::SetRotation(float radians)
{
	rotation.y = radians;
}

glm::vec3 SpatialObject::GetScale()
{
	return scale;
}

void SpatialObject::SetScale(const glm::vec3 &scaleFactor)
{
	scale = scaleFactor;
}