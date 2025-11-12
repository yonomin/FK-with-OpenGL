#ifndef STATICPOSTURE_H
#define STATICPOSTURE_H

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <functional>
#include "boneInfo.h"
#include "bone.h"
#include "model.h"
#include "assimpNodeData.h"
#include "GLFW/glfw3.h"

enum rotationAxis
{
	x_axis,
	y_axis,
	z_axis
};

class Posture
{
public:
	//旋转模式
	bool rotationMode{ false };
private:
	AssimpNodeData m_RootNode;//静态姿势最终展示时的节点
	std::map<std::string, BoneInfo> boneInfoMap;//模型的boneInfoMap

	int boneCount;

	std::vector<glm::mat4> m_FinalLocalBoneMatrices;
	std::vector<glm::mat4> m_FinalGlobalBoneMatrices;

	glm::mat4 t_LocalTransform;//临时矩阵
	std::vector<glm::mat4> m_CurrentLocalBoneMatrices;

	

public:
	inline void processRotationInput(GLFWwindow *ptr_window, Posture &posture, BoneIndicatorUse &indicatorUse)
	{
		if (indicatorUse.chosingBone == false)
			return;
		bool ctrlPressed = (glfwGetKey(ptr_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
		double rotationAmount = ctrlPressed ? -1.0 : 1.0;
		if (glfwGetKey(ptr_window, GLFW_KEY_X) == GLFW_PRESS)
		{
			posture.UpdatePosture(indicatorUse.chosenBoneID, rotationAxis::x_axis, rotationAmount);
		}
		else if (glfwGetKey(ptr_window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			posture.UpdatePosture(indicatorUse.chosenBoneID, rotationAxis::y_axis, rotationAmount);
		}
		else if (glfwGetKey(ptr_window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			posture.UpdatePosture(indicatorUse.chosenBoneID, rotationAxis::z_axis, rotationAmount);
		}

	}

	Posture(Model *model)
	{
		m_RootNode = model->GetRootNode();
		boneInfoMap = model->GetBoneInfoMap();//直接从model接受骨骼和节点信息
		boneCount = model->GetBoneCount();
		m_CurrentLocalBoneMatrices = model->GetTransformations();

		m_FinalLocalBoneMatrices.reserve(boneCount);
		for (int i = 0; i < boneCount; i++)
			m_FinalLocalBoneMatrices.push_back(glm::mat4(1.0f));//为本类m_FinalBoneMatrices变量储存100个默认骨骼矩阵

		m_FinalGlobalBoneMatrices.reserve(boneCount);
		for (int i = 0; i < boneCount; i++)
			m_FinalGlobalBoneMatrices.push_back(glm::mat4(1.0f));

		CalculateBoneTransform(&m_RootNode, glm::mat4(1.0f));
	}


	//根据节点的固有transformation，填充m_RootNode所有节点
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;//m_CurrentLocalBoneMatrices应该是没问题的，为什么这句代码去掉->transformation就会出bug
		//因为每个node都有transform，但不是每个node都在boneinfomap里

		if (boneInfoMap.find(nodeName) != boneInfoMap.end())//检查当前节点名是否存在于boneInfoMap中,而不是Armature那种不是骨骼的节点,防止id=0的骨骼（腰部）被污染
		{
		///*debug*/std::cout << nodeName <<"    ID: " << boneInfoMap[nodeName].id << std::endl;
		///*debug*/PrintGLM::Mat4Print(nodeTransform);
		int id = boneInfoMap[nodeName].id;
		nodeTransform = m_CurrentLocalBoneMatrices[id];

		///*debug*/PrintGLM::Mat4Print(nodeTransform); std::cout <<"== == == == == == == == == == == == == == " << std::endl;

		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())//检查当前节点名是否存在于boneInfoMap中,而不是Armature那种不是骨骼的节点,防止id=0的骨骼（腰部）被污染
		{
			int index = boneInfoMap[nodeName].id;
			m_FinalGlobalBoneMatrices[index] = globalTransformation;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalLocalBoneMatrices[index] = globalTransformation * offset;
		}
		for (int i = 0; i < node->childrenCount; i++)//遍历节点的所有子节点
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}


	void UpdatePosture(int ID, rotationAxis axis,float radians)
	{
		glm::vec3 rotation_axis;
		switch (axis) 
		{
		case(x_axis):
			rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f); 
			break;
		case(y_axis):
			rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case(z_axis):
			rotation_axis = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		default:
			rotation_axis = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		m_CurrentLocalBoneMatrices[ID] = glm::rotate(m_CurrentLocalBoneMatrices[ID], glm::radians(radians), rotation_axis);
		CalculateBoneTransform(&m_RootNode, glm::mat4(1.0f));
	}

	const std::vector<glm::mat4>& GetFinalLocalBoneMatrices() { return m_FinalLocalBoneMatrices; }
	const std::vector<glm::mat4>& GetFinalGlobalBoneMatrices() { return m_FinalGlobalBoneMatrices; }

private:
};


#endif // !STATICPOSTURE_H
