#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

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

		void MainLoop();

		void Cleanup();

		GLFWwindow* window_;
		const int kWindowWidth = 800;
		const int kWindowHeight = 600;
	};
}
