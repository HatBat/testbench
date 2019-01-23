#include <stdio.h>
#include "VulkanRenderer.h"
#include <GL/glew.h>
#include <vulkan/vulkan.h>
#include <iostream>

#include "MaterialVk.h"
#include "MeshVk.h"
#include "../Technique.h"
#include "ResourceBindingVk.h"
#include "RenderStateVk.h"
#include "VertexBufferVk.h"
#include "ConstantBufferVk.h"
#include "Texture2DVk.h"

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}

int VulkanRenderer::shutdown()
{
	//SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

Mesh* VulkanRenderer::makeMesh() { 
	return new MeshVk(); 
}

Texture2D* VulkanRenderer::makeTexture2D()
{
	return (Texture2D*)new Texture2DVk();
}

Sampler2D* VulkanRenderer::makeSampler2D()
{
	return (Sampler2D*)new Sampler2DVk();
}

ConstantBuffer* VulkanRenderer::makeConstantBuffer(std::string NAME, unsigned int location) { 
	return new ConstantBufferVk(NAME, location);
}

std::string VulkanRenderer::getShaderPath() {
	return std::string("..\\assets\\GL45\\");
}

std::string VulkanRenderer::getShaderExtension() {
	return std::string(".glsl");
}

VertexBuffer* VulkanRenderer::makeVertexBuffer( size_t size, VertexBuffer::DATA_USAGE usage) { 
	return new VertexBufferVk(size, usage); 
};

Material* VulkanRenderer::makeMaterial(const std::string& name) { 
	return new MaterialVk(name);
}

Technique* VulkanRenderer::makeTechnique(Material* m, RenderState* r) {
	Technique* t = new Technique(m, r);
	return t;
}

RenderState* VulkanRenderer::makeRenderState() { 
	RenderStateVk* newRS = new RenderStateVk();
	newRS->setGlobalWireFrame(&this->globalWireframeMode);
	newRS->setWireFrame(false);
	return (RenderState*)newRS;
}

void VulkanRenderer::setWinTitle(const char* title) {
	SDL_SetWindowTitle(this->window, title);
}

int VulkanRenderer::initialize(unsigned int width, unsigned int height) {

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
			exit(-1);
	}

	window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN);

	/* Vulkan test code */
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::cout << extensionCount << " extensions supported" << std::endl;
	/* --- */
	
	//if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	//{
	//	fprintf(stderr, "%s", SDL_GetError());
	//	exit(-1);
	//}
	//// Request an OpenGL 4.5 context (should be core)
	//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	//// Also request a depth buffer
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	//context = SDL_GL_CreateContext(window);

	//SDL_GL_MakeCurrent(window, context);

	//int major, minor;
	//SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	//SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);

	//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	//glClearDepth(1.0f);
	//glDepthFunc(GL_LEQUAL);

	//glViewport(0, 0, width, height);

	//glewExperimental = GL_TRUE;
	//GLenum err = glewInit();
	//if (GLEW_OK != err)
	//{
	//	fprintf(stderr, "Error GLEW: %s\n", glewGetErrorString(err));
	//}

	return 0;
}

/*
 Super simplified implementation of a work submission
 TODO.
*/

int perMatVk = 1;
void VulkanRenderer::submit(Mesh* mesh) 
{
	if (perMatVk) {
		drawList2[mesh->technique].push_back(mesh);
	}
	else
		drawList.push_back(mesh);
};

/*
 Naive implementation, no re-ordering, checking for state changes, etc.
 TODO.
*/
void VulkanRenderer::frame() 
{
	if (perMatVk !=1) {

		for (auto mesh : drawList)
		{
			mesh->technique->enable(this);
			size_t numberElements = mesh->geometryBuffers[0].numElements;
			//glBindTexture(GL_TEXTURE_2D, 0);
			for (auto t : mesh->textures)
			{
				// we do not really know here if the sampler has been
				// defined in the shader.
				t.second->bind(t.first);
			}
			for (auto element : mesh->geometryBuffers) {
				mesh->bindIAVertexBuffer(element.first);
			}
			mesh->txBuffer->bind(mesh->technique->getMaterial());
			//glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numberElements);
		}
		drawList.clear();
	}
	else 
	{
		for (auto work : drawList2)
		{
			work.first->enable(this);
			for (auto mesh : work.second)
			{
				size_t numberElements = mesh->geometryBuffers[0].numElements;
				//glBindTexture(GL_TEXTURE_2D, 0);
				for (auto t : mesh->textures)
				{
					// we do not really know here if the sampler has been
					// defined in the shader.
					t.second->bind(t.first);
				}
				for (auto element : mesh->geometryBuffers) {
					mesh->bindIAVertexBuffer(element.first);
				}
				mesh->txBuffer->bind(work.first->getMaterial());
				//glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numberElements);
			}
		}
		drawList2.clear();
	}
};

void VulkanRenderer::present()
{
	SDL_GL_SwapWindow(window);
};


void VulkanRenderer::setClearColor(float r, float g, float b, float a)
{
	//glClearColor(r, g, b, a);
};

void VulkanRenderer::clearBuffer(unsigned int flag) 
{
	
};

//void VulkanRenderer::setRenderTarget(RenderTarget* rt) {};

void VulkanRenderer::setRenderState(RenderState* ps)
{
	// naive implementation
	ps->set();
};
