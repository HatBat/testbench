#include <windows.h>
#include <streambuf>
#include <sstream>
#include <istream>
#include <fstream>
#include <vector>
#include <set>
#include <assert.h>

#include "MaterialVk.h"

typedef unsigned int uint;

/*
	vtx_shader is the basic shader text coming from the .vs file.
	strings will be added to the shader in this order:
	// - version of GLSL
	// - defines from map
	// - existing shader code
*/
std::vector<std::string> MaterialVk::expandShaderText(std::string& shaderSource, ShaderType type)
{
	//std::vector<std::string> input_definitions;
	//std::ifstream includeFile("IA.h");
	//std::string includeFileString((std::istreambuf_iterator<char>(includeFile)), std::istreambuf_iterator<char>());
	//includeFile.close();
	std::vector<std::string> result{ "\n\n #version 450\n\0" };
	for (auto define : shaderDefines[type])
		result.push_back(define);
	result.push_back(shaderSource);
	return result;
};

MaterialVk::MaterialVk(const std::string& _name)
{ 
	isValid = false;
	name = _name;
};

MaterialVk::~MaterialVk() 
{ 
	// delete attached constant buffers
	for (auto buffer : constantBuffers)
	{
		if (buffer.second != nullptr)
		{
			delete(buffer.second);
			buffer.second = nullptr;
		}
	}
};

void MaterialVk::setDiffuse(Color c)
{
	
}

void MaterialVk::setShader(const std::string& shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		removeShader(type);
	}
	shaderFileNames[type] = shaderFileName;
};

// this constant buffer will be bound every time we bind the material
void MaterialVk::addConstantBuffer(std::string name, unsigned int location)
{
	constantBuffers[location] = new ConstantBufferVk(name, location);
}

// location identifies the constant buffer in a unique way
void MaterialVk::updateConstantBuffer(const void* data, size_t size, unsigned int location)
{
	constantBuffers[location]->setData(data, size, this, location);
}

void MaterialVk::removeShader(ShaderType type)
{
	
};

int MaterialVk::compileShader(ShaderType type, std::string& errString)
{
	return -1;
}

int MaterialVk::compileMaterial(std::string& errString)
{
	return -1;
};

int MaterialVk::enable() {
	return -1;
};

void MaterialVk::disable() {
	
};
