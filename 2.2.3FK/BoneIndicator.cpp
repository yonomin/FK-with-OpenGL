

#include "GlfwInputCollector.h"
#include "BoneIndicatorUse.h"
BoneIndicatorUse::BoneIndicatorUse()
{
}

void BoneIndicatorUse::withGLFWInput(GlfwInputCollector &GlfwInputCollector)
{
	ptr_glfwInputCollector = &GlfwInputCollector;

	//blockGLFWMouseInput();
	//isBoneIndicatorSelected();
}

//Private

//鼠标指针模式切换器
//void BoneIndicator::blockGLFWMouseInput()
//{
//	//不捕捉的条件
//	if (hoveringBone)
//	{
//		ptr_glfwInputCollector->toCaptureCursor = false;
//	}
//}

void BoneIndicatorUse::whenSelected()
{
	//初次选择
	if (ptr_glfwInputCollector->leftClickJustPressed
		&& hoveringOverBone
		&& !chosingBone
		)
	{
		chosingBone = true;
		chosenBoneID = hoveredBoneID;
	}
	//切换选择
	else if (ptr_glfwInputCollector->leftClickJustPressed
		&& hoveringOverBone
		&& chosingBone
		&& hoveredBoneID != chosenBoneID
		)
	{
		chosenBoneID = hoveredBoneID;
	}
	else if (ptr_glfwInputCollector->leftClickJustPressed
		&& hoveringOverBone
		&& chosingBone
		&& hoveredBoneID == chosenBoneID
		)
	{
		chosenBoneID = -1;
		chosingBone = false;
	}
}