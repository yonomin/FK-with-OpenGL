#pragma once
class GlfwUse;
class GlfwInputCollector;
struct ImGuiIO;
class ImguiWithOpenglGlfw
{
public:
	bool ioWantUseMouse{ false };
private:
	ImGuiIO *ptr_imguiIO{nullptr};
	GlfwUse &ref_glfwUse;
	GlfwInputCollector &ref_glfwInputCollector;
public:
	//初始化，集成opengl与glfw环境
	//默认设置为中文字符集，启用键盘响应，浅色主题
	ImguiWithOpenglGlfw(GlfwUse &glfwUse,GlfwInputCollector &GlfwInputCollector);
	//imgui帧
	void newFrame(const char *imguiTitle);
	void endFrame();
	//io是否想用鼠标
	void ifIoWantUseMouse();
	//终结
	void terminate();
private:

};

