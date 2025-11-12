#pragma once
#include <glm/glm.hpp>
//#include <GLFW/glfw3.h> 
struct GLFWwindow;
class GlfwUse;
class OpenGLWithGladGlfw
{
public:
	bool openglInitialized{ false };
	//屏幕比例
	float aspectRatio{ 1.0f };
private:
	GlfwUse &ref_glfwUse;
public:
	glm::vec4 viewportSize;


public:
	//构造函数
	//加载glad
	//默认启用深度测试，启用透明度混合
	OpenGLWithGladGlfw(GlfwUse &glfwUse);
	//设置清除颜色，清除颜色和深度缓存
	void setClearColorAndClear(float R, float G, float B, float A);
	//设置屏幕比例
	void setAspectRatio();

	//回调函数
	//根据window大小更新viewPortSize
	void static frameBufferSizeCallback(GLFWwindow *ptr_window, int width, int height);

private:
	
};

