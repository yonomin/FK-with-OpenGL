
//#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "GlfwUse.h"
#include "GlfwInputCollector.h"
#include "ProcessInput.h"
GlfwInputCollector::GlfwInputCollector(GLFWwindow &ptr_window) :
	ref_window(ptr_window)
{
}

void GlfwInputCollector::update()
{
	//常规更新
	regularUpdate();
	//鼠标
	leftKeyClickCheck();
	cursorCaptureSwitch();
	updateCursorCaptureState();

	//功能
	notCaptureMouseWhenPressingESC();
	//按键检测
	R_keyCheck();
	W_keyCheck();
	S_keyCheck();
	A_keyCheck();
	D_keyCheck();
	SPACE_keyCheck();
	LSHIFT_keyCheck();
	ESCAPE_keyCheck();

	//regularUpdate();
}

//void GlfwInputCollector::withCameraOpenGL(CameraOpenGL &camera)
//{
//	ptr_camera = &camera;
//	cursorOffsetToCamera();
//}
void GlfwInputCollector::cursorPosCallback(GLFWwindow *ptr_window, double xpos, double ypos)
{
	//创造userpointer
	windowContext *context = static_cast<windowContext *>(glfwGetWindowUserPointer(ptr_window));
	if (context && context->contextInputHandler)
	{
		//获取引用
		double &xPosLast = context->contextInputHandler->xPosLastFrame;
		double &yPosLast = context->contextInputHandler->yPosLastFrame;
		double &xPosCurrent = context->contextInputHandler->xPosCurrentFrame;
		double &yPosCurrent = context->contextInputHandler->yPosCurrentFrame;
		double &xPosOffset = context->contextInputHandler->xPosOffsetBetweenTwoFrames;
		double &yPosOffset = context->contextInputHandler->yPosOffsetBetweenTwoFrames;
		bool &moved = context->contextInputHandler->cursoreMovedInThisFrame;

		moved = true;
		////保存当前帧位置数据
		//xPosLast = xPosCurrent;
		//yPosLast = yPosCurrent;
		//更新当前帧位置数据
		xPosCurrent = xpos;
		yPosCurrent = ypos;

		//本帧为捕捉首帧
		if (context->contextInputHandler->firstFrameOfCapturingCursor)
		{
			//offset归零防鼠标瞬移
			xPosOffset = 0.0f;
			yPosOffset = 0.0f;
			context->contextInputHandler->firstFrameOfCapturingCursor = false;
		}
		//本帧非首帧
		else
		{
			//正常更新offset
			xPosOffset = xPosCurrent - xPosLast;
			yPosOffset = yPosLast - yPosCurrent;
		}
		//鼠标是否移动 
		//保存当前帧位置数据
		xPosLast = xPosCurrent;
		yPosLast = yPosCurrent;

	}
}

void GlfwInputCollector::scrollCallBack(GLFWwindow *ptr_window, double xoffset, double yoffset)
{
	windowContext *context = static_cast<windowContext *>(glfwGetWindowUserPointer(ptr_window));
	if (context && context->contextInputHandler)
	{
		//如果鼠标指针不在捕获状态
		if (!context->contextInputHandler->toCaptureCursor)
			return;
		context->contextInputHandler->yOffsetScroll = yoffset;
	}
}

//Private


//左键状态检测 每帧切换一次
void GlfwInputCollector::leftKeyClickCheck()
{
	//if (!enableClickInput)return;
	//检测左键当前状态
	leftClickCurrentIsDown = glfwGetMouseButton(&ref_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	//如果当前按下 上一帧是松开
	if (leftClickCurrentIsDown && !leftClickLastFrameIsDown)
	{
		leftClickLastFrameIsDown = true;
		leftClickJustPressed = true;
	}
	//如果当前是松开 上一帧是按下
	else if (!leftClickCurrentIsDown && leftClickLastFrameIsDown)
	{
		leftClickLastFrameIsDown = false;
		leftClickJustReleased = true;
	}
	//如果当前帧和上一帧状态相同
	//重置左键状态
	else if (leftClickCurrentIsDown == leftClickLastFrameIsDown)
	{
		leftClickJustPressed = false;
		leftClickJustReleased = false;
	}
}

void GlfwInputCollector::cursorCaptureSwitch()
{
	//如果需要显示指针
	if (!toCaptureCursor)
		//切换到显示指针模式
		glfwSetInputMode(&ref_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//若不需要显示指针
	else if (toCaptureCursor)
		//切换到锁定指针模式
		glfwSetInputMode(&ref_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

//鼠标指针锁定模式初始化器
void GlfwInputCollector::updateCursorCaptureState()
{
	//指针被捕捉时
	if (toCaptureCursor&& !wasCapturingCursorLastFrame)
	{
		//首次捕捉=true
		firstFrameOfCapturingCursor = true;
	}
	else
	{
		firstFrameOfCapturingCursor = false;
	}
	wasCapturingCursorLastFrame = toCaptureCursor;
}

void GlfwInputCollector::regularUpdate()
{
	cursoreMovedInThisFrame = false;
}


void GlfwInputCollector::notCaptureMouseWhenPressingESC()
{
	if (ESCAPE_KeyCurrentIsDown)
		toCaptureCursor = false;
}

void GlfwInputCollector::R_keyCheck()
{
	R_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_R) == GLFW_PRESS;
	if (R_KeyCurrentIsDown && !R_KeyLastFrameIsDown)
	{
		R_KeyLastFrameIsDown = true;
		R_KeyJustPressed = true;
	}
	else if (!R_KeyCurrentIsDown && R_KeyLastFrameIsDown)
	{
		R_KeyLastFrameIsDown = false;
		R_KeyJustReleased = true;
	}
	else if (R_KeyCurrentIsDown == R_KeyLastFrameIsDown)
	{
		R_KeyJustPressed = false;
		R_KeyJustReleased = false;
	}
}
void GlfwInputCollector::W_keyCheck()
{
	W_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_W) == GLFW_PRESS;
	if (W_KeyCurrentIsDown && !W_KeyLastFrameIsDown)
	{
		W_KeyLastFrameIsDown = true;
		W_KeyJustPressed = true;
	}
	else if (!W_KeyCurrentIsDown && W_KeyLastFrameIsDown)
	{
		W_KeyLastFrameIsDown = false;
		W_KeyJustReleased = true;
	}
	else if (W_KeyCurrentIsDown == W_KeyLastFrameIsDown)
	{
		W_KeyJustPressed = false;
		W_KeyJustReleased = false;
	}
}
void GlfwInputCollector::S_keyCheck()
{
	S_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_S) == GLFW_PRESS;
	if (S_KeyCurrentIsDown && !S_KeyLastFrameIsDown)
	{
		S_KeyLastFrameIsDown = true;
		S_KeyJustPressed = true;
	}
	else if (!W_KeyCurrentIsDown && S_KeyLastFrameIsDown)
	{
		S_KeyLastFrameIsDown = false;
		S_KeyJustReleased = true;
	}
	else if (S_KeyCurrentIsDown == S_KeyLastFrameIsDown)
	{
		S_KeyJustPressed = false;
		S_KeyJustReleased = false;
	}
}
void GlfwInputCollector::A_keyCheck()
{
	A_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_A) == GLFW_PRESS;
	if (A_KeyCurrentIsDown && !A_KeyLastFrameIsDown)
	{
		A_KeyLastFrameIsDown = true;
		A_KeyJustPressed = true;
	}
	else if (!A_KeyCurrentIsDown && A_KeyLastFrameIsDown)
	{
		A_KeyLastFrameIsDown = false;
		A_KeyJustReleased = true;
	}
	else if (A_KeyCurrentIsDown == A_KeyLastFrameIsDown)
	{
		A_KeyJustPressed = false;
		A_KeyJustReleased = false;
	}
}
void GlfwInputCollector::D_keyCheck()
{
	D_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_D) == GLFW_PRESS;
	if (D_KeyCurrentIsDown && !D_KeyLastFrameIsDown)
	{
		D_KeyLastFrameIsDown = true;
		D_KeyJustPressed = true;
	}
	else if (!D_KeyCurrentIsDown && D_KeyLastFrameIsDown)
	{
		D_KeyLastFrameIsDown = false;
		D_KeyJustReleased = true;
	}
	else if (D_KeyCurrentIsDown == D_KeyLastFrameIsDown)
	{
		D_KeyJustPressed = false;
		D_KeyJustReleased = false;
	}
}
void GlfwInputCollector::SPACE_keyCheck()
{
	SPACE_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (SPACE_KeyCurrentIsDown && !SPACE_KeyLastFrameIsDown)
	{
		SPACE_KeyLastFrameIsDown = true;
		SPACE_KeyJustPressed = true;
	}
	else if (!SPACE_KeyCurrentIsDown && SPACE_KeyLastFrameIsDown)
	{
		SPACE_KeyLastFrameIsDown = false;
		SPACE_KeyJustReleased = true;
	}
	else if (SPACE_KeyCurrentIsDown == SPACE_KeyLastFrameIsDown)
	{
		SPACE_KeyJustPressed = false;
		SPACE_KeyJustReleased = false;
	}
}
void GlfwInputCollector::LSHIFT_keyCheck()
{
	LSHIFT_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	if (LSHIFT_KeyCurrentIsDown && !LSHIFT_KeyLastFrameIsDown)
	{
		LSHIFT_KeyLastFrameIsDown = true;
		LSHIFT_KeyJustPressed = true;
	}
	else if (!LSHIFT_KeyCurrentIsDown && LSHIFT_KeyLastFrameIsDown)
	{
		LSHIFT_KeyLastFrameIsDown = false;
		LSHIFT_KeyJustReleased = true;
	}
	else if (LSHIFT_KeyCurrentIsDown == LSHIFT_KeyLastFrameIsDown)
	{
		LSHIFT_KeyJustPressed = false;
		LSHIFT_KeyJustReleased = false;
	}
}
void GlfwInputCollector::ESCAPE_keyCheck()
{
	ESCAPE_KeyCurrentIsDown = glfwGetKey(&ref_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
	if (ESCAPE_KeyCurrentIsDown && !ESCAPE_KeyLastFrameIsDown)
	{
		ESCAPE_KeyLastFrameIsDown = true;
		ESCAPE_KeyJustPressed = true;
	}
	else if (!ESCAPE_KeyCurrentIsDown && ESCAPE_KeyLastFrameIsDown)
	{
		ESCAPE_KeyLastFrameIsDown = false;
		ESCAPE_KeyJustReleased = true;
	}
	else if (ESCAPE_KeyCurrentIsDown == ESCAPE_KeyLastFrameIsDown)
	{
		ESCAPE_KeyJustPressed = false;
		ESCAPE_KeyJustReleased = false;
	}
}
