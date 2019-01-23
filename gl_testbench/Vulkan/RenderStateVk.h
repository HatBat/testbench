#pragma once
#include <vector>
#include "../RenderState.h"

class RenderStateVk : public RenderState
{
public:
	RenderStateVk();
	~RenderStateVk();
	void setWireFrame(bool);
	void set();

	void setGlobalWireFrame(bool* global);
private:
	bool _wireframe;
	bool* globalWireFrame;
};

