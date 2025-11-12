
//内部库
#include <iostream>
#include <memory>
//外部库

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
//自己的头文件
#include "GlfwInputCollector.h"
#include "OpenGLWithGLADGLFW.h"
#include "GlfwUse.h"
GlfwUse::GlfwUse(int versionMajor,int versionMinor, unsigned int winWidth, unsigned int winHeight, const char *title):
	ContextVersionMajor(versionMajor),
	ContextVersionMinor(versionMinor),
	defaultWidth(winWidth),
	defaultHeight(winHeight),
	WIN_TITLE(title)
{
	if(!glfwInit())
	{
		std::cout << "Failed to glfwInit" << std::endl;
		glfwTerminate();
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	ptr_window = glfwCreateWindow(winWidth, winHeight, title, NULL, NULL);
	//创建不成功
	if (!ptr_window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

}


//void GlfwUse::setInputHandler(GlfwInputCollector& handler)
//{
//	ptr_glfwInputCollector = &handler;
//}
//
//void GlfwUse::setOpenGL(OpenGLWithGladGlfw &gl)
//{
//	ptr_OpenGL = &gl;
//}

//void GlfwUse::endOfFrame() const
//{
//	//交换前后缓冲区
//	glfwSwapBuffers(ptr_window);
//	glfwPollEvents();//
//}


void GlfwUse::windowContextInitializeAndSetCallbacks(OpenGLWithGladGlfw &gl, GlfwInputCollector &input, GlfwUse &glfwUse)
{
	ptr_OpenGL = &gl;
	ptr_glfwInputCollector = &input;
	//填充上下文结构
	contextOwner = std::make_unique<windowContext>();
	contextOwner->contextOpenGL = ptr_OpenGL;
	contextOwner->contextInputHandler = ptr_glfwInputCollector;
	contextOwner->contextGlfwUse = &glfwUse;
	//设置userpointer
	glfwSetWindowUserPointer(ptr_window, contextOwner.get());
	//viewport回调函数
	glfwSetFramebufferSizeCallback(ptr_window, OpenGLWithGladGlfw::frameBufferSizeCallback);
	//指针位置回调函数
	glfwSetCursorPosCallback(ptr_window, GlfwInputCollector::cursorPosCallback);
	//滚轮回调函数
	glfwSetScrollCallback(ptr_window, GlfwInputCollector::scrollCallBack);
}

void GlfwUse::startTimer()
{
	timeOfCurrentFrame = glfwGetTime();
	deltaTimeBetweenTowFrames = timeOfCurrentFrame - timeOfLastFrame;
	timeOfLastFrame = timeOfCurrentFrame;
}

//接收回调函数
//void GlfwUse::acceptCallBackFunctions() const
//{
//	//viewport回调函数
//	glfwSetFramebufferSizeCallback(window, OpenGLWithGladGlfw::frameBufferSizeCallback);
//	//指针位置回调函数
//	glfwSetCursorPosCallback(window, GlfwInputCollector::cursorPosCallback);
//	//滚轮回调函数
//	glfwSetScrollCallback(window, GlfwInputCollector::scrollCallBack);
//}

//Private

//void GlfwUse::cursorCaptureSwitcher()
//{
//	if(ptr_glfwInputCollector->toCaptureCursor)
//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//	else if(!ptr_glfwInputCollector->toCaptureCursor)
//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//}
