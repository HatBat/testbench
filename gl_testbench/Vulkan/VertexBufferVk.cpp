#include "VertexBufferVk.h"
#include "MeshVk.h"
#include <assert.h>

/*
	Create a VertexBuffer backed in gpu mem
	- todo: add error checking...
*/
VertexBufferVk::VertexBufferVk(size_t size, DATA_USAGE usage) {
	totalSize = size;
}

VertexBufferVk::~VertexBufferVk()
{
	
}

/*
	SSBO based Vertex Buffer
*/
void VertexBufferVk::setData(const void* data, size_t size,  size_t offset)
{
	assert(size + offset <= totalSize);
}

/*
 bind at "location", with offset "offset", "size" bytes 
 */
void VertexBufferVk::bind(size_t offset, size_t size, unsigned int location) {
	assert(offset + size <= totalSize);
}

inline void VertexBufferVk::unbind() {
	
}

inline size_t VertexBufferVk::getSize() {
	return totalSize;
}
