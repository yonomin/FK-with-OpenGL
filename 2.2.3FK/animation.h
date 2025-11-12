#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "bone.h"
#include <functional>
#include "boneInfo.h"
#include "model.h"



class Animation
{
public:
	//Animation构造函数
	//最终装填m_Duration和m_TicksPerSecond两个成员变量 并读取?
	Animation(const std::string &animationPath, Model *model)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];//mAnimations中的第一个动画 
		m_Duration = animation->mDuration;//获取该动画的tick数
		m_TicksPerSecond = animation->mTicksPerSecond;//每秒tick数
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;//根节点的在自身模型坐标系中的初始姿势
		//globalTransformation = globalTransformation.Inverse();//初始姿态的逆矩阵(好像没用)
		ReadHierarchyData(m_RootNode, scene->mRootNode);//src读数据到&dest
		ReadMissingBones(animation, *model); //给animation类的m_BoneInfoMap补充动画需要的model中没有的骨骼 ?既然model中没有，补充的骨骼就不会绑定到模型的任何顶点上吧？

	}

	//查找动画所有通道的骨骼数组中，是否由符合参数名称的骨骼，返回这个骨骼的地址
	Bone *FindBone(const std::string &name)
	{
		//查找动画所有通道的骨骼数组中，是否由符合参数名称的骨骼，得到这个骨骼的迭代器
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone &Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;//没找到就返回空指针
		else return &(*iter);//找到就返回这个骨骼地址
	}

	inline float GetDuration() { return m_Duration; }
	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline const AssimpNodeData &GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo> &GetBoneIDMap() { return m_BoneInfoMap; }

private:
	float m_Duration;//一个动画的tick数
	int m_TicksPerSecond;//一个动画的每秒tick数
	std::vector<Bone> m_Bones;//动画所有通道的骨骼数组
	AssimpNodeData m_RootNode;//动画要用的模型的节点信息的备份
	std::map<std::string, BoneInfo> m_BoneInfoMap;//给模型补齐m_BoneInfoMap后复制过来的一份

	//最终将aiNode的成员变量信息依次搬运到m_RootNode
	void ReadHierarchyData(AssimpNodeData &dest, const aiNode *src)//
	{
		assert(src);

		dest.name = src->mName.data;//将aiNode的mName存入一个AssimpNodeData的name变量中
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);////将aiNode的mTransformation存入一个AssimpNodeData的transformation变量中
		dest.childrenCount = src->mNumChildren;////将aiNode的子节点数mNumChildren存入一个AssimpNodeData的子节点数变量中

		for (int i = 0; i < src->mNumChildren; i++)//遍历这个aiNode的子节点
		{
			AssimpNodeData newData;//创建一个新的AssimpNodeData
			ReadHierarchyData(newData, src->mChildren[i]);//递归操作每一个子节点
			dest.children.push_back(newData);//填充进循环外的节点的children变量中
		}
	}

	//为模型补齐动画需要但是模型不具备的骨骼
	//填充动画所有通道的骨骼数组m_Bones
	void ReadMissingBones(const aiAnimation *animation, Model &model) //将model中boneInfoMap缺失的，与channel的mNodeName对不上的bone加入到本类的m_BoneInfoMap中来
	{
		int size = animation->mNumChannels;//读取aiAnimation对象的动画通道数

		auto &boneInfoMap = model.GetBoneInfoMap();//获取模型的boneInfoMap
		int &boneCount = model.GetBoneCount(); //获取Model对象的boneCount

		for (int i = 0; i < size; i++)//遍历所有通道
		{
			auto channel = animation->mChannels[i];//每个通道
			std::string boneName = channel->mNodeName.data;//获取该动画通道需要的骨骼名 每个动画通道仅影响一个骨骼

			if (boneInfoMap.find(boneName) == boneInfoMap.end())//如果boneInfoMap中没有动画所需的骨骼
			{
				boneInfoMap[boneName].id = boneCount;//boneCount在传入时就会比id大1
				boneCount++;//model类内骨骼总数增加
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));//为animation类的m_Bones存入每个通道的所需骨骼
		}

		m_BoneInfoMap = boneInfoMap;//备份一份所需所需模型的m_BoneInfoMap到本自己类的成员对象m_BoneInfoMap
	}
};
#endif // !ANIMATION_H
