#pragma once
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h> 
struct GLFWwindow;
class CameraOpenGL;
class GlfwInputCollector
{
	//变量== == == == == == == == == == == == == == 
public:
	//全局状态
	bool enableClickInput           { true };
	//捕捉鼠标
	bool toCaptureCursor            { false };
	//是否是捕捉鼠标的第一帧
	bool firstFrameOfCapturingCursor{ false };
	bool wasCapturingCursorLastFrame{ false };
	bool cursoreMovedInThisFrame{};
	//指针在window中的位置
	double xPosCurrentFrame{ 0.0f }, yPosCurrentFrame{ 0.0f };
	double xPosLastFrame   { 0.0f }, yPosLastFrame   { 0.0f };
	double xPosOffsetBetweenTwoFrames{ 0.0f }, yPosOffsetBetweenTwoFrames{ 0.0f };
	//鼠标滚轮
	double xOffsetScroll{ 0.0f }, yOffsetScroll{ 0.0f };


private:
	GLFWwindow &ref_window;
	//CameraOpenGL *ptr_camera;
	//函数== == == == == == == == == == == == == == 
public:
	//构造函数
	explicit GlfwInputCollector(GLFWwindow &ptr_window);

	//更新输入
	void update();

	//回调函数
	//鼠标指针位置回调函数
	static void cursorPosCallback(GLFWwindow *ptr_window, double xpos, double ypos);
	//鼠标滚轮回调函数
	static void scrollCallBack(GLFWwindow *ptr_window, double xoffset, double yoffset);



private:
	//鼠标
	//左键状态检测 每帧切换一次
	void leftKeyClickCheck();
	//GLFW鼠标捕捉的开关
	void cursorCaptureSwitch();
	//鼠标指针锁定模式初始化器
	void updateCursorCaptureState();
	//常规更新状态
	void regularUpdate();
	//键盘
	void R_keyCheck();
	void W_keyCheck();
	void A_keyCheck();
	void S_keyCheck();
	void D_keyCheck();
	void SPACE_keyCheck();
	void LSHIFT_keyCheck();
	void ESCAPE_keyCheck();

	//功能
	//切换鼠标显示
	void notCaptureMouseWhenPressingESC();


public:
	//按键检测
	//鼠标
	//左键状态检测
	bool leftClickCurrentIsDown{ false };
	bool leftClickLastFrameIsDown{ false };
	bool leftClickJustPressed{ false };
	bool leftClickJustReleased{ false };
	//键盘
	bool R_KeyCurrentIsDown{ false };
	bool R_KeyLastFrameIsDown{ false };
	bool R_KeyJustPressed{ false };
	bool R_KeyJustReleased{ false };

	bool W_KeyCurrentIsDown{ false };
	bool W_KeyLastFrameIsDown{ false };
	bool W_KeyJustPressed{ false };
	bool W_KeyJustReleased{ false };

	bool A_KeyCurrentIsDown{ false };
	bool A_KeyLastFrameIsDown{ false };
	bool A_KeyJustPressed{ false };
	bool A_KeyJustReleased{ false };

	bool S_KeyCurrentIsDown{ false };
	bool S_KeyLastFrameIsDown{ false };
	bool S_KeyJustPressed{ false };
	bool S_KeyJustReleased{ false };

	bool D_KeyCurrentIsDown{ false };
	bool D_KeyLastFrameIsDown{ false };
	bool D_KeyJustPressed{ false };
	bool D_KeyJustReleased{ false };

	bool SPACE_KeyCurrentIsDown{ false };
	bool SPACE_KeyLastFrameIsDown{ false };
	bool SPACE_KeyJustPressed{ false };
	bool SPACE_KeyJustReleased{ false };

	bool LSHIFT_KeyCurrentIsDown{ false };
	bool LSHIFT_KeyLastFrameIsDown{ false };
	bool LSHIFT_KeyJustPressed{ false };
	bool LSHIFT_KeyJustReleased{ false };

	bool ESCAPE_KeyCurrentIsDown{ false };
	bool ESCAPE_KeyLastFrameIsDown{ false };
	bool ESCAPE_KeyJustPressed{ false };
	bool ESCAPE_KeyJustReleased{ false };
};

