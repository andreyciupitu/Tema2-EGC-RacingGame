#pragma once

#include <RacingGame/RenderableObject.h>
#include <RacingGame/SpatialObject.h>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>

class Road : public RenderableObject
{
public:
	Road(const glm::vec3& startPos, float width, int drawRange);
	virtual ~Road();

	// Eliminates road tiles that are no longer visible
	// and adds new segments at the end of the current mesh
	void Update(glm::vec3 cameraPos, glm::vec3 direction);

	virtual glm::mat4 GetModel() override;

	// Returns a set of waypoints that describe the current state of the road
	const std::vector<glm::vec3>& GetKeyPoints();
	const std::vector<SpatialObject*>& GetRoadBorders();

	float GetWidth();
	void SetWidth(float w);

private:
	enum SectionType
	{
		LEFT, RIGHT, FORWARD
	};

	void GenerateSegment(int segmentNumber);

private:
	std::vector<VertexFormat> rightSide;
	std::vector<VertexFormat> leftSide;
	std::vector<glm::vec3> keyPoints;
	std::vector<SpatialObject*> borders;
	float width;
	float deviation;
	glm::vec3 start;
	int planned;
	int drawRange;
	glm::vec3 lastPos;
	SectionType planned_type;

	// Angle of deviation for each segment (in degrees)
	const float ANGLE_STEP = 5.0f;
};

