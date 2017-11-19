#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <RacingGame/SpatialObject.h>

#define FLOAT_EPS 0.01f

namespace Geometry3D
{
	struct Projection
	{
		float left, right;
		glm::vec3 axis;
	};

	Mesh* CreateQuad(std::string name, float length, const glm::vec3 &color);

	// Collision Detection using SAT algorithm.
	// The algorithm projects the vertices of both objects on the same axis
	// and checks for intersections.
	glm::vec3 CheckCollision(SpatialObject* o1, SpatialObject* o2);

	// Projects an object specified by the given vertices on an axis, returns the versor of the axis
	// and the extremities of the projection.
	// @param vertices - the positions of the vertices of the object
	// @param axis - the axis to project on
	Projection ProjectObjectOnAxis(const std::vector<glm::vec3> &vertices, const glm::vec3 &axis);

} // namespace Geometry3D