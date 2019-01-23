#pragma once

#include <GL/glew.h>

#include "../Texture2D.h"
#include "Sampler2DVk.h"


class Texture2DVk :
	public Texture2D
{
public:
	Texture2DVk();
	~Texture2DVk();

	int loadFromFile(std::string filename);
	void bind(unsigned int slot);
};

