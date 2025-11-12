#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"

class Animator
{
public:
	//为Animator对象的三个私有成员变量初始化
	Animator(Animation *animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
		m_FinalLocalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; i++)
			m_FinalLocalBoneMatrices.push_back(glm::mat4(1.0f));//为本类m_FinalBoneMatrices变量储存100个默认骨骼矩阵
		m_FinalGlobalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; i++)
			m_FinalGlobalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	//根据时间（秒）计算当前动画中所有aiNode在当前时间戳的姿态,结果存入成员变量m_FinalBoneMatrices，当前进度条位置(tick)存入成员变量m_CurrentTime
	void UpdateAnimation(float dt)//将骨骼更新到当前位置
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)//如果m_CurrentAnimation内存指向一个有效的animation对象
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	//计算当前动画中所有aiNode在当前时间戳的姿态,结果存入成员变量m_FinalBoneMatrices
	void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform)//计算骨骼变换
	{
		std::string nodeName = node->name;//获取节点名称
		glm::mat4 nodeTransform = node->transformation;//获取节点变换

		Bone *Bone = m_CurrentAnimation->FindBone(nodeName);//获取当前动画的 与当前节点对应的Animation对象的骨骼指针

		if (Bone)//如果骨骼指针指向一个地址
		{
			Bone->Update(m_CurrentTime);//将animation中对应的Bone更新至当前时间戳的姿态
			nodeTransform = Bone->GetLocalTransform();//获得这个姿态
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;//当前的Tick最终姿态，先引用子变换再引用父节点

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();//获取当前动画所需的BoneIDMap
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())//如果能在map中找到节点名对应的值
		{
			int id = boneInfoMap[nodeName].id;//读取骨骼id，即当前node的id
			m_FinalGlobalBoneMatrices[id] = globalTransformation;//最终骨骼全局姿态
			glm::mat4 offset = boneInfoMap[nodeName].offset;//读取骨骼offset
			m_FinalLocalBoneMatrices[id] = globalTransformation * offset;//最终骨骼局部姿态
		}

		for (int i = 0; i < node->childrenCount; i++)//遍历节点的所有子节点
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}
	
	//从0tick开始播放动画（?好像没用到这个函数）
	//void PlayAnimation(Animation *pAnimation)
	//{
	//	m_CurrentAnimation = pAnimation;
	//	m_CurrentTime = 0.0f;
	//}

	//返回成员变量m_FinalLocalBoneMatrices
	std::vector<glm::mat4> GetFinalLocalBoneMatrices() { return m_FinalLocalBoneMatrices; }
	std::vector<glm::mat4> GetFinalGlobalBoneMatrices() { return m_FinalGlobalBoneMatrices; }
private:
	std::vector<glm::mat4> m_FinalLocalBoneMatrices;//当前channel中相应aiNode在当前tick的姿态矩阵，保存顺序是骨骼id
	std::vector<glm::mat4> m_FinalGlobalBoneMatrices;
	Animation *m_CurrentAnimation;//当前动画
	float m_CurrentTime;//动画的当前进度条位置（tick）
	float m_DeltaTime;//与前一个时间的时间差（秒）
};
#endif // !ANIMATOR_H
