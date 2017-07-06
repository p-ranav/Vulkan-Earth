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

	};
}
