#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "GlfwUse.h"
#include "OpenGLWithGladGlfw.h"
OpenGLWithGladGlfw::OpenGLWithGladGlfw(GlfwUse &glfwUse):
	ref_glfwUse(glfwUse),
	viewportSize(0.0f, 0.0f, (float)ref_glfwUse.defaultWidth, (float)ref_glfwUse.defaultHeight)
{
	glfwMakeContextCurrent(ref_glfwUse.ptr_window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	openglInitialized = true;
	//test部分初始化
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLWithGladGlfw::setClearColorAndClear(float R, float G, float B, float A)
{
	glClearColor(R, G, B, A);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLWithGladGlfw::setAspectRatio()
{
	//如果viewport大于0，即viewport存在
	if (viewportSize.w > 0.0f)
		aspectRatio = viewportSize.z / viewportSize.w;
	//viewport消失 ，比如被隐藏到后台
	else
		aspectRatio = (float)ref_glfwUse.defaultWidth / (float)ref_glfwUse.defaultHeight;
}

void OpenGLWithGladGlfw::frameBufferSizeCallback(GLFWwindow *ptr_window, int width, int height)
{
	//创造userpointer
	windowContext *context = static_cast<windowContext *>(glfwGetWindowUserPointer(ptr_window));
	if (context&&context->contextOpenGL)
		context->contextOpenGL->viewportSize = glm::vec4(0.0f, 0.0f, width, height);
	//if (context && context->contextGlfwUse)
	//{
	//	context->contextGlfwUse->defaultWidth = width;
	//	context->contextGlfwUse->defaultHeight = height;
	//}
}
//private

