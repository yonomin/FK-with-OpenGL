#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "GlfwInputCollector.h"
#include "GlfwUse.h"
#include "ImguiWithOpenglGlfw.h"
ImguiWithOpenglGlfw::ImguiWithOpenglGlfw(GlfwUse &glfwUse, GlfwInputCollector &glfwInputCollector):
	ref_glfwUse(glfwUse),
	ref_glfwInputCollector(glfwInputCollector)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ptr_imguiIO = &io;
	//更换字体为中文
	io.Fonts->Clear();
	static const ImWchar ranges[] = {
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x4E00, 0x9FA5, // CJK Unified Ideographs (中日韩统一表意文字)
		0x3000, 0x30FF, // CJK Symbols and Punctuation, Hiragana, Katakana
		0,              // 终止符
	};
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 26.0f, NULL, ranges);
	io.Fonts->Build();
	//启用键盘响应
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//浅色主题
	ImGui::StyleColorsLight();
	//集成glfw
	ImGui_ImplGlfw_InitForOpenGL(ref_glfwUse.ptr_window, true);
	//使用的glsl版本
	ImGui_ImplOpenGL3_Init("#version 430");
}

void ImguiWithOpenglGlfw::newFrame(const char* imguiTitle)
{
	//从glfw获得本帧的事件与窗口状态
	ImGui_ImplGlfw_NewFrame();
	//准备渲染所需的状态
	ImGui_ImplOpenGL3_NewFrame();
	//准备构建ui
	ImGui::NewFrame();
	ImGui::Begin(imguiTitle);
}

void ImguiWithOpenglGlfw::endFrame()
{
	//结束当前的 ImGui 窗口定义
	ImGui::End();
	//转化为顶点缓冲和绘制命令列表
	ImGui::Render();
	//绘制
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiWithOpenglGlfw::ifIoWantUseMouse()
{
	if (ptr_imguiIO->WantCaptureMouse)
		ioWantUseMouse = true;
	else if (!(ptr_imguiIO->WantCaptureMouse))
		ioWantUseMouse = false;
}

void ImguiWithOpenglGlfw::terminate()
{
	//关闭 OpenGL 渲染后端
	ImGui_ImplOpenGL3_Shutdown();
	//关闭 GLFW 平台后端
	ImGui_ImplGlfw_Shutdown();
	//销毁 ImGui 核心上下文
	ImGui::DestroyContext();
}
//private
