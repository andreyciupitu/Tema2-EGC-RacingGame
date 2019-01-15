#include "Road.h"

#include <vector>
#include <RacingGame/Transform.h>

Road::Road(const glm::vec3& startPos, float width, int drawRange) : start(startPos), width(width), drawRange(drawRange)
{
	modelMatrix = Transform::Translate(startPos);
	planned = 0;
	lastPos = startPos;
	mesh = new Mesh("street");

	rightSide = std::vector<VertexFormat>(drawRange, VertexFormat(glm::vec3(0)));
	leftSide = std::vector<VertexFormat>(drawRange, VertexFormat(glm::vec3(0)));
	keyPoints = std::vector<glm::vec3>(drawRange);

	// Initialize borders
	borders = std::vector<SpatialObject*>(2 * drawRange);
	for (int i = 0; i < borders.size(); i++)
	{
		borders[i] = new SpatialObject();
		borders[i]->SetMesh(MeshManager::GetMesh("box"));
		borders[i]->SetBaseColor(glm::vec3(0.933, 0.447, 0.498));
		borders[i]->SetScale(glm::vec3(1, 0.4f, 0.5f));
		borders[i]->SetTransform(glm::vec3(0, 0.2f, 0));
	}

	// Generate the first set of segments
	for (int i = 0; i < drawRange; i++)
	{
		GenerateSegment(i);
		mesh->vertices.push_back(leftSide[i]);
		mesh->vertices.push_back(rightSide[i]);
	}

	// Position the borders
	for (int j = 0; j < drawRange; j++)
	{
		borders[2 * j]->SetTransform(leftSide[j].position + glm::vec3(0, 0.2f, 0));
		borders[2 * j + 1]->SetTransform(rightSide[j].position + glm::vec3(0, 0.2f, 0));
	}
	
	for (int i = 3; i < 2 *drawRange; i++)
	{
		mesh->indices.push_back(i - 3);
		mesh->indices.push_back(i - 2);
		mesh->indices.push_back(i - 1);

		mesh->indices.push_back(i - 1);
		mesh->indices.push_back(i - 2);
		mesh->indices.push_back(i);
	}
	mesh->UseMaterials(false);
	mesh->InitFromData(mesh->vertices, mesh->indices);
}

Road::~Road()
{
	delete mesh;
	for (auto b : borders)
		delete b;
}

void Road::Update(glm::vec3 cameraPos, glm::vec3 direction)
{
	int i = 0;

	// Remove segments that have been passed
	while (i < drawRange && glm::dot(keyPoints[i] - cameraPos, glm::vec3(direction.x, 0, direction.z)) <= -2)
		i++;

	if (i == 0)
		return;

	for (int j = i; j < drawRange; j++)
	{
		rightSide[j - i] = rightSide[j];
		leftSide[j - i] = leftSide[j];
		keyPoints[j - i] = keyPoints[j];
	}

	// Spawn new segments at the end of the road
	for (int j = drawRange - i; j < drawRange; j++)
		GenerateSegment(j);

	// Re-adjust the borders
	for (int j = 0; j < drawRange; j++)
	{
		borders[2 * j]->SetTransform(leftSide[j].position + glm::vec3(0, 0.2f, 0));
		borders[2 * j + 1]->SetTransform(rightSide[j].position + glm::vec3(0, 0.2f, 0));
	}

	mesh->vertices.clear();
	for (int j = 0; j < drawRange; j++)
	{
		mesh->vertices.push_back(leftSide[j]);
		mesh->vertices.push_back(rightSide[j]);
	}
	mesh->InitFromData(mesh->vertices, mesh->indices);
}

glm::mat4 Road::GetModel()
{
	return modelMatrix;
}

const std::vector<glm::vec3>& Road::GetKeyPoints()
{
	return keyPoints;
}

const std::vector<SpatialObject*>& Road::GetRoadBorders()
{
	return borders;
}

float Road::GetWidth()
{
	return width;
}

void Road::SetWidth(float w)
{
	width = w;
}

void Road::GenerateSegment(int segmentNumber)
{
	glm::vec3 currentPos = lastPos;

	// Plan a new section (Left, Right, Forward)
	if (planned == 0)
	{
		planned_type = static_cast<SectionType> (rand() % 3);
		planned = rand() % 5 + 5;
	}

	planned--;

	// If we need to draw a curve increase the deviation
	if (planned_type == LEFT)
		deviation += ANGLE_STEP * M_PI / 180; //convert to radians
	else if (planned_type == RIGHT)
	{
		deviation -= ANGLE_STEP * M_PI / 180;
	}
	
	// Add the current position to the road description
	keyPoints[segmentNumber] = currentPos;

	// Compute the edges of the new segment
	glm::vec3 posRight = currentPos + glm::vec3(-1 * width / 2 * sin(deviation), 0.0f, width / 2 * cos(deviation));
	glm::vec3 posLeft = currentPos + glm::vec3(width / 2 * sin(deviation), 0.0f, -1 * width / 2 * cos(deviation));

	rightSide[segmentNumber] = VertexFormat(posRight, glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(segmentNumber % 2, 0.0f));
	leftSide[segmentNumber] = VertexFormat(posLeft, glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(segmentNumber % 2, 1.0f));

	// Remember the next position
	currentPos += glm::vec3(cos(deviation), 0.0f, sin(deviation)) * 2.0f;

	lastPos = currentPos;
}
