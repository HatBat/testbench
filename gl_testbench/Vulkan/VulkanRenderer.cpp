#define NOMINMAX

#include <stdio.h>
#include "VulkanRenderer.h"
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include "MaterialVk.h"
#include "MeshVk.h"
#include "../Technique.h"
#include "ResourceBindingVk.h"
#include "RenderStateVk.h"
#include "VertexBufferVk.h"
#include "ConstantBufferVk.h"
#include "Texture2DVk.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

// static functions

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}

int VulkanRenderer::shutdown()
{
	cleanup();
	
	//SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
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

	windowWidth = width;
	windowHeight = height;

	window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN);

	initVulkan();

	///* Vulkan test code */
	//uint32_t extensionCount = 0;
	//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	//std::cout << extensionCount << " extensions supported" << std::endl;
	///* --- */
	
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

// private methods
void VulkanRenderer::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	selectPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void VulkanRenderer::cleanup()
{
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::createInstance()
{
	if (enableValidationLayers && !validationLayersAreSupported())
	{
		throw std::runtime_error("Validation layer not available!");
	}
	
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getExtensions();

	createInfo.enabledExtensionCount = (uint32_t)extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance!");
	}
}

void VulkanRenderer::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

void VulkanRenderer::createSurface()
{
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	{
		throw std::runtime_error("Failed to create surface!");
	}
}

void VulkanRenderer::selectPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (deviceIsSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find suitable physical device!");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = (uint32_t)(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentsQueue);
}

void VulkanRenderer::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

VkSurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { windowWidth, windowHeight };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VulkanRenderer::SwapChainSupportDetails VulkanRenderer::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool VulkanRenderer::deviceIsSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);
	
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainIsAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainIsAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainIsAdequate;
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	for (const auto& extensionProperty : availableExtensions)
	{
		if (strcmp(extensionProperty.extensionName, deviceExtensions[0]) == 0)
		{
			return true;
		}
	}

	return false;
}

VulkanRenderer::QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool VulkanRenderer::validationLayersAreSupported()
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const auto& layerProperties : availableLayers)
	{
		if (strcmp(layerProperties.layerName, validationLayers[0]) == 0)
		{
			return true;
		}
	}
	
	return false;
}

std::vector<const char*> VulkanRenderer::getExtensions()
{
	unsigned int extensionCount;
	if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr))
	{
		throw std::runtime_error("Failed to get instance extension count!");
	}

	std::vector<const char*> extensions = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME  // Additional extension
	};
	size_t additionalExtentionCount = extensions.size();
	extensions.resize(extensionCount + additionalExtentionCount);

	if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data() + additionalExtentionCount))
	{
		throw std::runtime_error("Failed to get instance extensions!");
	}

	return extensions;
}
