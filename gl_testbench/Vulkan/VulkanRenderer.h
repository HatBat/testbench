#pragma once

#include "../Renderer.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include <optional>
#include <array>

//#include <GL/glew.h>
//#include <SDL_opengl.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")



class VulkanRenderer : public Renderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	Material* makeMaterial(const std::string& name);
	Mesh* makeMesh();
	//VertexBuffer* makeVertexBuffer();
	VertexBuffer* makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage);
	ConstantBuffer* makeConstantBuffer(std::string NAME, unsigned int location);
//	ResourceBinding* makeResourceBinding();
	RenderState* makeRenderState();
	Technique* makeTechnique(Material* m, RenderState* r);
	Texture2D* makeTexture2D();
	Sampler2D* makeSampler2D();
	std::string getShaderPath();
	std::string getShaderExtension();

	int initialize(unsigned int width = 640, unsigned int height = 480);
	void setWinTitle(const char* title);
	int shutdown();

	void setClearColor(float, float, float, float);
	void clearBuffer(unsigned int);
//	void setRenderTarget(RenderTarget* rt); // complete parameters
	void setRenderState(RenderState* ps);
	void submit(Mesh* mesh);
	void frame();
	void present();

private:
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct Vertex {
		glm::vec4 pos;
		//glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			/*attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);*/

			return attributeDescriptions;
		}
	};

	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 dummy;
	};

	/*const std::vector<Vertex> vertices = {
		{ { 0.0f, -0.5f },{ 1.0f, 1.0f, 1.0f } },
		{ { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
	};*/

	SDL_Window* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	//VkBuffer vertexBuffer;
	//VkDeviceMemory vertexBufferMemory;
	VertexBuffer* posVertexBuffer;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	uint32_t windowWidth = 0;
	uint32_t windowHeight = 0;
	VkClearValue clearColor;

	//SDL_GLContext context;

	std::vector<Mesh*> drawList;
	std::unordered_map<Technique*, std::vector<Mesh*>> drawList2;

	uint32_t numMeshesToRender = 0;
	std::vector<glm::vec4> meshTranslations;
	
	bool globalWireframeMode = false;

	//int initializeOpenGL(int major, int minor, unsigned int width, unsigned int height);
	//float clearColor[4] = { 0,0,0,0 };
	/*std::unordered_map<int, int> BUFFER_MAP = { 
		{0, 0},
		{CLEAR_BUFFER_FLAGS::COLOR, GL_COLOR_BUFFER_BIT },
		{CLEAR_BUFFER_FLAGS::DEPTH, GL_DEPTH_BUFFER_BIT }, 
		{CLEAR_BUFFER_FLAGS::STENCIL, GL_STENCIL_BUFFER_BIT }
	};*/

private:
	void initVulkan();
	void cleanupSwapChain();
	void cleanup();
	void recreateSwapChain();
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void selectPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createVertexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createCommandBuffers();
	void recordCommandBuffers();
	void createSyncObjects();
	void updateUniformBuffer(uint32_t currentImage);
	void drawFrame();

	VkShaderModule createShaderModule(const std::vector<char>& code);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool deviceIsSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	bool validationLayersAreSupported();
	std::vector<const char*> getExtensions();
};

