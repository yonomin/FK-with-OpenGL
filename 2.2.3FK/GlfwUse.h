#pragma once
#include <memory>

class GlfwInputCollector;
class OpenGLWithGladGlfw;
class GlfwUse;
struct GLFWwindow;
struct windowContext
{
	OpenGLWithGladGlfw *contextOpenGL;
	GlfwUse *contextGlfwUse;
	GlfwInputCollector *contextInputHandler;
};

class GlfwUse
{
	//变量== == == == == == == == == == == == == == 
public:
	GLFWwindow *ptr_window{ nullptr };
	std::unique_ptr<windowContext> contextOwner;
	//窗口
	unsigned int defaultWidth{ 0 };
	unsigned int defaultHeight{ 0 };
	const char *WIN_TITLE{ "NULL" };
	//计时
	double timeOfCurrentFrame{  };
	double timeOfLastFrame{  };
	double deltaTimeBetweenTowFrames{  };
private:
	//上下文
	GlfwInputCollector *ptr_glfwInputCollector{ nullptr };
	OpenGLWithGladGlfw *ptr_OpenGL{ nullptr };
	int ContextVersionMajor{ -1 };
	int ContextVersionMinor{ -1 };

	//函数== == == == == == == == == == == == == == 
public:
	//初始胡glfw并创建窗口，
	//默认GLFW_OPENGL_CORE_PROFILE, monitor:NULL , share:NULL
	GlfwUse(int versionMajor, int versionMinor, unsigned int winWidth, unsigned int winHeight, const char *title);

	////操作器接收器
	//void setInputHandler(GlfwInputCollector &handler);
	////openg接收器
	//void setOpenGL(OpenGLWithGladGlfw &OpenGL);
	/*void endOfFrame() const;*/
	//windowsContext初始化
	void windowContextInitializeAndSetCallbacks(OpenGLWithGladGlfw &gl, GlfwInputCollector &input, GlfwUse &glfwUse);
	//接收回调函数
	/*void acceptCallBackFunctions() const;*/

	//计时器
	void startTimer();
private:
	//捕获指针切换器
	//void cursorCaptureSwitcher();
};

