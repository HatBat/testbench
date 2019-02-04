#pragma once
#include <GL/glew.h>
#include "../VertexBuffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>

class VertexBufferVk :
	public VertexBuffer
{
public:
	
	VertexBufferVk(size_t size, VertexBuffer::DATA_USAGE usage, VkPhysicalDevice pd, VkDevice ld, VkQueue gq, VkCommandPool cp);
	~VertexBufferVk();
	
	void setData(const void* data, size_t size, size_t offset);
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();
	VkBuffer getVertexBuffer();

private:

	size_t totalSize;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkCommandPool commandPool;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

private:
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

