#ifndef BONE_H
#define BONE_H


#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "assimp_glm_helpers.h"
#include <vector>
#include <assimp/scene.h>
#include <list>

struct KeyPosition//节点在一个关键帧的位置
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation//一个关键帧时的旋转
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale//一个关键帧时的缩放
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{

public:
	glm::mat4 m_LocalTransform;//当前node在当前animation的对应channel中，位于animationTime这个时间戳的姿态
private:
	std::vector<KeyPosition> m_Positions;//一个channel中node的所有位置数据，和mNumPositionKeys顺序一致
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;//定义aiNode平移（Translation）变化的关键帧的数量
	int m_NumRotations;
	int m_NumScalings;

	std::string m_Name;//?当前aiNodeAnim的mName，也是aiNode的mName
	int m_ID;//ID

public:
	//一个骨骼的构造函数 
	// 填充了骨骼的m_Positions  m_Rotations  m_Scales  m_NumPositions  m_NumRotations  m_NumScalings
	//描述了一个ainode在动画的对应channel中的每一个关键帧的姿态
	Bone(const std::string &name, int ID, const aiNodeAnim *channel) :
		m_Name(name),
		m_ID(ID),
		m_LocalTransform(1.0f)
	{
		m_NumPositions = channel->mNumPositionKeys;
		for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)//遍历每个m_NumPositions
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;//当前位置变量的位置值
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;//当前Position  mTime
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			m_Positions.push_back(data);
		}

		m_NumRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;
			m_Rotations.push_back(data);
		}//同理

		m_NumScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = channel->mScalingKeys[keyIndex].mTime;
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(scale);
			data.timeStamp = timeStamp;
			m_Scales.push_back(data);
		}
	}

	Bone(const std::string &name, int ID, glm::mat4 LocalTransform ) :
		m_Name(name),
		m_ID(ID),
		m_LocalTransform(LocalTransform)
	{}

	Bone() ://默认构造函数
		m_Name(""),
		m_ID(-1),
		m_LocalTransform(1.0f),
		m_NumPositions(0),
		m_NumRotations(0),
		m_NumScalings(0)
	{}

	//最终当前node在当前animation的对应channel中，位于animationTime这个时间戳的姿态应用到公共成员变量m_LocalTransform
	void Update(float animationTime)
	{
		glm::mat4 translation = InterpolatePosition(animationTime);//当前时间戳animationTime的translation矩阵
		glm::mat4 rotation = InterpolateRotation(animationTime);//当前时间戳animationTime的rotation矩阵
		glm::mat4 scale = InterpolateScaling(animationTime);//当前时间戳animationTime的scale矩阵
		m_LocalTransform = translation * rotation * scale;//当前时间戳animationTime的最终姿态矩阵
	}
	//返回当前node在当前animation的对应channel中，位于animationTime这个时间戳的姿态
	glm::mat4 GetLocalTransform() { return m_LocalTransform; }
	std::string GetBoneName() const { return m_Name; }
	int GetBoneID() { return m_ID; }

	//时间戳animationTime与位置关键帧中的所有时间戳的tick数比较
	//最终返回时间戳animationTime之前的位置关键帧所在的时间戳序号
	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < m_NumPositions - 1; ++index)//遍历动画中所有位置,除了最后一个
		{
			if (animationTime < m_Positions[index + 1].timeStamp)//如果该时间戳animationTime的tick数小于任一位置关键帧的时间戳
				return index;//返回时间戳animationTime的前一个戳
		}
		assert(0);
	}
	//最终返回时间戳animationTime之间的旋转关键帧的时间戳序号
	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}
	//最终返回时间戳animationTime之间的缩放关键帧的时间戳序号
	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < m_NumScalings - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

private:
	//最终返回在animationTime位置插值后的Position矩阵
	glm::mat4 InterpolatePosition(float animationTime)
	{
		if (1 == m_NumPositions)//如果channel中只有一个位置
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);//则返回一个应用了这个位置的矩阵

		int p0Index = GetPositionIndex(animationTime);//时间戳animationTime之前的关键帧时间戳序号
		int p1Index = p0Index + 1;//时间戳animationTime之后的关键帧时间戳序号
		float scaleFactor = GetScaleFactor(//时间戳插值权重
			m_Positions[p0Index].timeStamp,m_Positions[p1Index].timeStamp, animationTime);
		glm::vec3 finalPosition = glm::mix(//当前时间戳的Position向量
			m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);//返回当前时间戳的位置矩阵FinalPosition
	}
	//最终返回在animationTime位置插值后的Rotation矩阵
	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (1 == m_NumRotations)
		{
			auto rotation = glm::normalize(m_Rotations[0].orientation);
			return glm::toMat4(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
			m_Rotations[p1Index].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
			, scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);

	}
	//最终返回在animationTime位置插值后的Scaling矩阵
	glm::mat4 InterpolateScaling(float animationTime)
	{
		if (1 == m_NumScalings)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
			m_Scales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
			, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

	//最终返回时间戳插值权重，即当前时间戳于前后关键帧时间戳构成的时间戳区间的比值
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;//当前时间戳与前一个关键帧时间戳的tick数量差
		float framesDiff = nextTimeStamp - lastTimeStamp;//当前时间戳所在的前后两个关键帧时间戳之间的tick数量差
		scaleFactor = midWayLength / framesDiff;//时间戳插值权重，即当前时间戳于前后关键帧时间戳构成的时间戳区间的比值
		return scaleFactor;//返回时间戳插值权重
	}
};



#endif // !BONE_H
