#include "Engine.h"

void Engine::Renderer::InitWindow()
{
	// Initializes the GLFW library
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(kWindowWidth, kWindowHeight,
		"Engine", nullptr, nullptr);
}

void Engine::Renderer::InitVulkan()
{
	CreateVulkanInstance();
}

void Engine::Renderer::CreateVulkanInstance()
{
	// Check if Vulkan Validation Layers are Available (if requested)
	if (kEnableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("Validation Layers requested, but not available!");
	}

	/*
	* ApplicationInfo is technically optional, but it may 
	* provide some useful information to the driver to 
	* optimize for our specific application
	*/
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	/*
	* This struct tells the Vulkan driver which global 
	* extensions and validation layers we want to use
	*/
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	/*
	* Vulkan is a platform agnostic API, which means that we need an 
	* extension to interface with the window system. GLFW has a 
	* handy built-in function that returns the extension(s)
	*/
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	/*
	* The general pattern that object creation function parameters in Vulkan follow is:
	* (1) Pointer to struct with creation info
	* (2) Pointer to custom allocator callbacks, always nullptr in this tutorial
	* (3) Pointer to the variable that stores the handle to the new object
	*/
	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

void Engine::Renderer::MainLoop()
{
	// GLFW Event Loop
	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
	}
}

void Engine::Renderer::Cleanup()
{

	vkDestroyInstance(vkInstance, nullptr);
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

bool Engine::Renderer::CheckValidationLayerSupport()
{
	// List all of the available extensions 
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	/*
	* Check if all of the layers in kValidationLayers 
	* exist in the availableLayers list
	*/
	for (const char* layerName : kValidationLayers) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}
	return true;
}
