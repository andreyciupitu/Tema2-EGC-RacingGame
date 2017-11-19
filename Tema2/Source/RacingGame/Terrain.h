#pragma once

#include <RacingGame/RenderableObject.h>

class Terrain : public RenderableObject
{
public:
	enum TerrainSize
	{
		TERRAIN_SIZE_SMALL = 2,
		TERRAIN_SIZE_MEDIUM = 5,
		TERRAIN_SIZE_LARGE = 10
	};

	Terrain()
	{
		height = 0.0f;
		size = TERRAIN_SIZE_SMALL;
		modelMatrix = Transform3D::Scale(glm::vec3(size));
	}

	~Terrain() {}

	float GetHeight() { return height; }

	TerrainSize GetSize() { return size; }
	void SetSize(TerrainSize s)
	{
		size = s;
		modelMatrix = Transform3D::Scale(glm::vec3(size));
	}

private:
	TerrainSize size;
	float height;
};