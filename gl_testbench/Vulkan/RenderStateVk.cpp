#include <GL/glew.h>
#include "RenderStateVk.h"

RenderStateVk::RenderStateVk()
{
	_wireframe = false;
}

RenderStateVk::~RenderStateVk()
{
}

void RenderStateVk::set()
{
	// was wireframe mode already set?
	if (*globalWireFrame == _wireframe)
		return;
	else
		*globalWireFrame = _wireframe;
}

/*
	Keep a pointer to the global wireframe state
*/
void RenderStateVk::setGlobalWireFrame(bool* global)
{
	this->globalWireFrame = global;
}

/*
 set wireframe mode for this Render state
*/
void RenderStateVk::setWireFrame(bool wireframe) {
	_wireframe = wireframe;

}
