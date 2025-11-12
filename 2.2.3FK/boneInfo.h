#ifndef BONE_INFO_H
#define BONE_INFO_H

#include<glm/glm.hpp>

//一个骨骼的信息结构体；包含两个变量:
//int id;
//glm::mat4 offset;
struct BoneInfo
{
	int id;
	glm::mat4 offset;
};
#endif // !BONE_INFO_H
