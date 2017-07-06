#include "Engine.h"

void Engine::Renderer::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window_ = glfwCreateWindow(kWindowWidth, kWindowHeight, 
		"Engine", nullptr, nullptr);
}

void Engine::Renderer::InitVulkan()
{
}

void Engine::Renderer::MainLoop()
{
	while (!glfwWindowShouldClose(window_)) {
		glfwPollEvents();
	}
}

void Engine::Renderer::Cleanup()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
}
