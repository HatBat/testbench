#pragma once
#include <GL/glew.h>
#include "../ConstantBuffer.h"

#include <glm/glm.hpp>

class ConstantBufferVk : public ConstantBuffer
{
public:
	ConstantBufferVk(std::string NAME, unsigned int location);
	~ConstantBufferVk();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);
	glm::vec4 getTranslation();

private:

	std::string name;
	GLuint location;
	GLuint handle;
	GLuint index;
	void* buff = nullptr;
	void* lastMat;

	glm::vec4 currentTranslation;
};

