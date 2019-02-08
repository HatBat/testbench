#include "ConstantBufferVk.h"
#include "MaterialVk.h"

ConstantBufferVk::ConstantBufferVk(std::string NAME, unsigned int location) 
{
	name = NAME;
	handle = 0;
}

ConstantBufferVk::~ConstantBufferVk()
{
	if (handle != 0)
	{
		handle = 0;
	};
}

// this allows us to not know in advance the type of the receiving end, vec3, vec4, etc.
void ConstantBufferVk::setData(const void* data, size_t size, Material* m, unsigned int location)
{
	memcpy(&currentTranslation, data, size);
}

void ConstantBufferVk::bind(Material* m)
{
	
}

glm::vec4 ConstantBufferVk::getTranslation()
{
	return currentTranslation;
}

