#pragma once
class GlfwInputCollector;
class ImguiWithOpenglGlfw;
class OpenGLWithGladGlfw;
class Camera;
class GlfwUse;
class BoneIndicator;
class BoneIndicatorUse;
class Posture;
class ProcessInput
{
public:
	ProcessInput(
		GlfwInputCollector &inputHandler, GlfwUse &glfwUse, ImguiWithOpenglGlfw &imgui,
		OpenGLWithGladGlfw &gl, Camera &camera, BoneIndicatorUse &indicator,Posture &posture);

	void update();

	bool cameraInputActive{};
private:
	GlfwInputCollector &ref_input;
	GlfwUse &ref_glfwUse;
	ImguiWithOpenglGlfw &ref_imgui;
	OpenGLWithGladGlfw &ref_opengl;
	Camera &ref_camera;
	BoneIndicatorUse &ref_indicator;
	Posture &ref_posture;
	//切换相机输入
	void cameraInputSwitcher();
	//相机移动
	void cameraMovement();
	//启用捕捉鼠标
	void whenToCaptureCursor();
	//相机处理鼠标
	void cameraWithMouse();
	//imgui的互动
	void blockGLFWMouseInput();
	//切换骨骼转动模式
	void rotationModeSwitch();
	//切换到选择模式
	void selectModeSwitch();
};

