#pragma once
#include "../Sampler2D.h"

class Sampler2DVk :
	public Sampler2D
{
public:
	Sampler2DVk();
	~Sampler2DVk();
	void setMagFilter(FILTER filter);
	void setMinFilter(FILTER filter);
	void setWrap(WRAPPING s, WRAPPING t);
private:
};

