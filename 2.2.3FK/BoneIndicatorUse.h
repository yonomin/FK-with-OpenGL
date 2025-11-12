#pragma once
class GlfwInputCollector;

class BoneIndicatorUse
{
	//变量== == == == == == == == == == == == == == 
public:
	//选择
	bool chosingBone{ false };
	int chosenBoneID{ -1 };
	//悬浮
	bool hoveringOverBone{ false };
	int hoveredBoneID{ -1 };
	////旋转模式
	//bool rotationMode{ false };
private: 
	GlfwInputCollector *ptr_glfwInputCollector{ nullptr };

	//函数== == == == == == == == == == == == == == 
public:
	//构造函数
	BoneIndicatorUse();

	void withGLFWInput(GlfwInputCollector &GlfwInputCollector);
private:
	//void blockGLFWMouseInput();
	void whenSelected();
};
