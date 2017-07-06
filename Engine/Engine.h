#pragma once

// External Headers
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// System Headers
#include <iostream>
#include <vector>
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

	};
}
