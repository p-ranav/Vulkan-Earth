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
	SetupDebugCallback();
}

void Engine::Renderer::CreateVulkanInstance()
{
	// Check if Vulkan Validation Layers are Available (if requested)
	if (kEnableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("Validation Layers not available!");
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
	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	// Add validation layers to createInfo struct (if requested)
	if (kEnableValidationLayers) {
		createInfo.enabledLayerCount = 
			static_cast<uint32_t>(kValidationLayers.size());
		createInfo.ppEnabledLayerNames = kValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	/*
	* The general pattern that object creation function 
	* parameters in Vulkan follow is:
	* (1) Pointer to struct with creation info
	* (2) Pointer to custom allocator callbacks, always nullptr
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
	DestroyDebugReportCallbackEXT(vkInstance, 
		vkDebugCallback, nullptr);
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

std::vector<const char*> Engine::Renderer::GetRequiredExtensions()
{
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

	/*
	* The extensions specified by GLFW are always required, 
	* but the debug report extension is conditionally added. 
	*/
	if (kEnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

// Debug Callback - Called by Vulkan and used by Validation Layers
VKAPI_ATTR VkBool32 VKAPI_CALL 
Engine::Renderer::DebugCallback(VkDebugReportFlagsEXT flags, 
	VkDebugReportObjectTypeEXT objType, 
	uint64_t obj, 
	size_t location, 
	int32_t code, 
	const char * layerPrefix, 
	const char * msg, 
	void * userData)
{
	std::cerr << "Validation Layer: " << msg << std::endl;
	return VK_FALSE;
}

void Engine::Renderer::SetupDebugCallback()
{
	// Need not bother if validation layers is disabled
	if (!kEnableValidationLayers) return;

	/*
	* DebugCallback Creation Struct
	* (1) The flags field allows us to filter which 
	* types of messages you would like to receive
	* (2) The pfnCallback field specifies the 
	* pointer to the callback function
	*/
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = 
		VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT 
		| VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = DebugCallback;

	if (CreateDebugReportCallbackEXT(vkInstance, &createInfo, 
		nullptr, &vkDebugCallback) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup Debug Callback!");
	}
}

/*
* Looks up vkCreateDebugReportCallbackEXT and 
* calls this function with the provided arguments
*/
VkResult Engine::Renderer::CreateDebugReportCallbackEXT(VkInstance instance, 
	const VkDebugReportCallbackCreateInfoEXT * pCreateInfo, 
	const VkAllocationCallbacks * pAllocator, 
	VkDebugReportCallbackEXT * pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)
		vkGetInstanceProcAddr(instance, 
			"vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Engine::Renderer::DestroyDebugReportCallbackEXT(VkInstance instance, 
	VkDebugReportCallbackEXT callback, 
	const VkAllocationCallbacks * pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)
		vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

void Engine::Renderer::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
	// If there are no devices with Vulkan support 
	// then there is no point going further
	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

	// Check suitability of each device
	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

// GPU device suitability check
bool Engine::Renderer::IsDeviceSuitable(VkPhysicalDevice device)
{
	/*
	* Sample device compability check:
	* We consider our application only usable for
	* dedicated graphics cards that support geometry shaders
	*/
	/*
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;
	*/
	return true;
}
