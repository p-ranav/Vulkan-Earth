#pragma once

// External Headers
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// System Headers
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <stdexcept>

namespace Engine {

	class Renderer {
	public:
		void Run() {
			InitWindow();
			InitVulkan();
			MainLoop();
			Cleanup();
		}
	private:

		void InitWindow();

		void InitVulkan();

		void CreateVulkanInstance();

		void MainLoop();

		void Cleanup();

	private:
		/* GLFW Window Object */
		GLFWwindow* pWindow;

		/* Window Sizing Constants */
		const int kWindowWidth = 800;
		const int kWindowHeight = 600;

		/* Vulkan Instance */
		VkInstance vkInstance;

		/* Vulkan Validation Layers */
		const bool kEnableValidationLayers = true;
		const std::vector<const char*> kValidationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};
		/*
		* Checks if all of the requested validation 
		* layers are available
		*/ 
		bool CheckValidationLayerSupport();

		/*
		* Returns the required list of extensions based on 
		* whether validation layers are enabled or not
		*/
		std::vector<const char*> GetRequiredExtensions();

		/*
		* Vulkan Debug Callback
		* The VKAPI_ATTR and VKAPI_CALL ensure that the function 
		* has the right signature for Vulkan to call it.
		*/
		VkDebugReportCallbackEXT vkDebugCallback;
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData);

		// Registers the above debug callback with Vulkan
		void SetupDebugCallback();

		/*
		* Since vkCreateDebugReportCallbackEXT function is an
		* extension function, it is not automatically loaded.
		* We have to look up its address ourselves 
		* using vkGetInstanceProcAddr
		*/
		VkResult CreateDebugReportCallbackEXT(VkInstance instance, 
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
			const VkAllocationCallbacks* pAllocator, 
			VkDebugReportCallbackEXT* pCallback);

		/*
		* Destroy the DebugReport Callback
		*/
		void DestroyDebugReportCallbackEXT(VkInstance instance,
			VkDebugReportCallbackEXT callback,
			const VkAllocationCallbacks* pAllocator);

		/*
		* Look for and select a graphics card in the system
		* that supports the features we need
		*/
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		void PickPhysicalDevice();
		/*
		* Evaluate input physical device for suitability
		*/
		bool IsDeviceSuitable(VkPhysicalDevice device);

		struct QueueFamilyIndices {
			int graphicsFamily = -1;
			int presentFamily = -1;

			bool isComplete() {
				return graphicsFamily >= 0 && presentFamily >= 0;
			}
		};
		/*
		* This function will return the indices of the queue families that 
		* satisfy certain desired properties. 
		* The best way to do that is using a structure, where 
		* an index of -1 will denote "not found":
		*/
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		/*
		* Class member to store the logical device handle in
		*/
		VkDevice logicalDevice;

		// Creates the Logical Device
		void CreateLogicalDevice();

		/*
		* The queues are automatically created along with the logical device
		* Device queues are implicitly cleaned up when the device is destroyed
		*/
		VkQueue graphicsQueue;
		
		/*
		* Since Vulkan is a platform agnostic API, it can not interface 
		* directly with the window system on its own. To establish the 
		* connection between Vulkan and the window system to present 
		* results to the screen, we need to use the WSI 
		* (Window System Integration) extensions
		*/
		VkSurfaceKHR surface;
		void CreateWindowSurface();

		/*
		* Presentation Queue
		*/
		VkQueue presentQueue;

		/*
		* A list of required device extensions, similar to 
		* the list of validation layers to enable
		*/
		const std::vector<const char*> kDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Called from IsDeviceSuitable as an additional check
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		// Swap Chain Support Details
		struct SwapChainSupportDetails {
			// Basic surface capabilities (min/max number of images in 
			// swap chain, min/max width and height of images)
			VkSurfaceCapabilitiesKHR capabilities;
			// Surface formats (pixel format, color space)
			std::vector<VkSurfaceFormatKHR> formats;
			// Available presentation modes
			std::vector<VkPresentModeKHR> presentModes;
		};

		/*
		* Just checking if a swap chain is available is not sufficient, 
		* because it may not actually be compatible with our window surface.
		* There are basically three kinds of properties we need to check:
		* (1) Basic surface capabilities (min/max number of images in 
		* swap chain, min/max width and height of images)
		* (2) Surface formats (pixel format, color space)
		* (3) Available presentation modes
		*/
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		/*
		* There are three types of settings to determine:
		* (1) Surface format (color depth)
		* (2) Presentation mode (conditions for "swapping" images to the screen)
		* (3) Swap extent (resolution of images in swap chain)
		* For each of these settings we'll have an ideal value in mind that we'll 
		* go with if it's available and otherwise we'll create some logic 
		* to find the next best thing.
		*/
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const 
			std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const 
			std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		/*
		* The main create swap chain method that uses the above ChooseSwapXYZ
		* methods to choose the right parameters for the swap chain
		*/
		VkSwapchainKHR swapChain;
		void CreateSwapChain();

		// Retrieved swap chain images
		std::vector<VkImage> swapChainImages;

		// Format and extent chosen for the swap chain images
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		// Swap Chain Image Views
		std::vector<VkImageView> swapChainImageViews;
		void CreateImageViews();



		// Read File Helper for Loading Shaders
		static std::vector<char> ReadFile(const std::string& filename);

		/*
		* The function will take a buffer with the bytecode as parameter 
		* and create a VkShaderModule from it
		*/
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		/*
		* We need to specify how many color and depth buffers there will be,
		* how many samples to use for each of them and how their contents
		* should be handled throughout the rendering operations.
		* All of this information is wrapped in a render pass object
		*/
		VkRenderPass renderPass;
		void CreateRenderPass();

		/* Pipeline Layout - uniform values in shaders specified here */
		VkPipelineLayout pipelineLayout;

		// Sets up the Graphics Pipeline
		VkPipeline graphicsPipeline;
		void CreateGraphicsPipeline();

		// Framebuffers
		std::vector<VkFramebuffer> swapChainFramebuffers;
		void CreateFramebuffers();

		// Command Pools
		VkCommandPool commandPool;
		void CreateCommandPool();

		// Command Buffer Allocation
		std::vector<VkCommandBuffer> commandBuffers;
		void CreateCommandBuffers();

		/*
		* The DrawFrame function will perform the following operations:
		* (1) Acquire an image from the swap chain
		* (2) Execute the command buffer with that image as attachment in the framebuffer
		* (3) Return the image to the swap chain for presentation
		*/
		void DrawFrame();

		// Semaphores for synchronization in drawing frames
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;
		void CreateSemaphores();
	};
}
