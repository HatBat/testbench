#pragma once
#include <GL/glew.h>
#include "../ConstantBuffer.h"

class ConstantBufferVk : public ConstantBuffer
{
public:
	ConstantBufferVk(std::string NAME, unsigned int location);
	~ConstantBufferVk();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);

private:

	std::string name;
	GLuint location;
	GLuint handle;
	GLuint index;
	void* buff = nullptr;
	void* lastMat;
};
