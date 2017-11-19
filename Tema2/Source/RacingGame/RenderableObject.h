#pragma once

#include <Core/Engine.h>

class RenderableObject
{
public:
	RenderableObject() {}
	virtual ~RenderableObject() {}

	Mesh* GetMesh() { return mesh; }
	void SetMesh(Mesh *mesh)
	{
		this->mesh = mesh;
		this->mesh->UseMaterials(false);
	}

	Shader* GetShader() { return shader; }
	void SetShader(Shader *shader) { this->shader = shader; }

	Texture2D* GetTexture() { return texture; }
	void SetTexture(Texture2D *texture) { this->texture = texture; }

	glm::vec3 GetBaseColor() { return baseColor; }
	void SetBaseColor(const glm::vec3 &color) { baseColor = color; }

	glm::mat4 GetModel() { return modelMatrix; }
	void SetModel(const glm::mat4 &model) { modelMatrix = model; }

protected:
	Mesh *mesh;
	Shader *shader;
	Texture2D * texture;
	glm::vec3 baseColor;
	glm::mat4 modelMatrix;
};
