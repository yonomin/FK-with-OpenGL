
#include "GlfwInputCollector.h"
#include "OpenGLWithGladGlfw.h"
#include "ImguiWithOpenglGlfw.h"
#include "camera.h"
#include "GlfwUse.h"
#include "ProcessInput.h"
#include "BoneIndicatorUse.h"
#include "Posture.h"
ProcessInput::ProcessInput(
	GlfwInputCollector &input, GlfwUse &glfwUse, ImguiWithOpenglGlfw &imgui,
	OpenGLWithGladGlfw &gl, Camera &camera, BoneIndicatorUse &indicator, Posture &posture) :
	ref_input(input),
	ref_glfwUse(glfwUse),
	ref_imgui(imgui),
	ref_opengl(gl),
	ref_camera(camera),
	ref_indicator(indicator),
	ref_posture(posture)
{
}
void ProcessInput::update()
{
	cameraInputSwitcher();
	cameraMovement();
	whenToCaptureCursor();
	cameraWithMouse();
	blockGLFWMouseInput();//?不加好像也没事看看为什么
	rotationModeSwitch();
	selectModeSwitch();
}

//private

void ProcessInput::cameraInputSwitcher()
{
	if (ref_input.toCaptureCursor)
		cameraInputActive = true;
	else
		cameraInputActive = false;
}

void ProcessInput::cameraMovement()
{
	if (!cameraInputActive) return;
	if (ref_input.W_KeyCurrentIsDown)
		ref_camera.ProcessKeyboard(FORWARD, ref_glfwUse.deltaTimeBetweenTowFrames);
	if (ref_input.S_KeyCurrentIsDown)
		ref_camera.ProcessKeyboard(BACKWARD, ref_glfwUse.deltaTimeBetweenTowFrames);
	if (ref_input.A_KeyCurrentIsDown)
		ref_camera.ProcessKeyboard(LEFT, ref_glfwUse.deltaTimeBetweenTowFrames);
	if (ref_input.D_KeyCurrentIsDown)
		ref_camera.ProcessKeyboard(RIGHT, ref_glfwUse.deltaTimeBetweenTowFrames);
	if (ref_input.SPACE_KeyCurrentIsDown)
		ref_camera.ProcessKeyboard(UP, ref_glfwUse.deltaTimeBetweenTowFrames);
	if (ref_input.LSHIFT_KeyCurrentIsDown)
		ref_camera.ProcessKeyboard(DOWN, ref_glfwUse.deltaTimeBetweenTowFrames);

}

void ProcessInput::whenToCaptureCursor()
{
	if (ref_input.leftClickJustPressed
		&& !ref_imgui.ioWantUseMouse
		&& !ref_indicator.hoveringOverBone)
	{
		ref_input.toCaptureCursor = true;
	}
}

void ProcessInput::cameraWithMouse()
{
	if (!ref_input.cursoreMovedInThisFrame) return;
	if (ref_input.toCaptureCursor)
	{
		ref_camera.ProcessMouseMovement(ref_input.xPosOffsetBetweenTwoFrames, ref_input.yPosOffsetBetweenTwoFrames);
	}
}
void ProcessInput::blockGLFWMouseInput()
{
	//当imgui的io要捕捉鼠标
	if (ref_imgui.ioWantUseMouse
		&& !ref_input.toCaptureCursor)
	{
		ref_input.toCaptureCursor = false;
		ref_input.enableClickInput = false;
	}
}

void ProcessInput::rotationModeSwitch()
{
	if(ref_input.R_KeyJustPressed
		&& ref_indicator.chosingBone
		&& !ref_posture.rotationMode)
		ref_posture.rotationMode = true;
	else if(ref_input.R_KeyJustPressed
		&& ref_indicator.chosingBone
		&& ref_posture.rotationMode)
		ref_posture.rotationMode = false;
}

void ProcessInput::selectModeSwitch()
{
	//初次选择
	if (ref_input.leftClickJustPressed
		&& ref_indicator.hoveringOverBone
		&& !ref_indicator.chosingBone)
	{
		ref_indicator.chosingBone = true;
		ref_indicator.chosenBoneID = ref_indicator.hoveredBoneID;
	}
	//切换选择
	else if (ref_input.leftClickJustPressed//切换选择
		&& ref_indicator.hoveringOverBone
		&& ref_indicator.chosingBone
		&& ref_indicator.hoveredBoneID != ref_indicator.chosenBoneID)
	{
		ref_indicator.chosenBoneID = ref_indicator.hoveredBoneID;
	}
	else if (ref_input.leftClickJustPressed//取消选择
		&& ref_indicator.hoveringOverBone
		&& ref_indicator.chosingBone)
	{
		ref_indicator.chosingBone = false;
		ref_indicator.chosenBoneID = -1;
	}
}

