#pragma once

#include <Core/Engine.h>
#include <RacingGame/RenderableObject.h>
#include <RacingGame/Transform3D.h>

class SpatialObject : public RenderableObject
{
public:
	SpatialObject() : transform(glm::vec3(0, 0, 0)), rotation(glm::vec3(0, 0, 0)), scale(glm::vec3(1, 1, 1)) {}
	virtual ~SpatialObject() {};

	// Update the parameters of the object between frames.
	virtual void Update(float deltaTimeSeconds) = 0;

	// Generate a modelMatrix rotating and scaling form the origin.
	// Override for a more complex behavior or for non-centered meshes.
	virtual glm::mat4 GetModelMatrix();

	glm::vec3 GetTransform();
	void SetTransform(const glm::vec3 &position);

	glm::vec3 GetRotation();
	void SetRotation(float radians);

	glm::vec3 GetScale();
	void SetScale(const glm::vec3 &scaleFactor);

protected:
	glm::vec3 transform;
	glm::vec3 rotation;
	glm::vec3 scale;
};