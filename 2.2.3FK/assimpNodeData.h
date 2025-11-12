#ifndef ASSIMP_NODE_DATA_H
#define ASSIMP_NODE_DATA_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
struct AssimpNodeData
{
	glm::mat4 transformation;//当前节点相较于其父节点的姿态变换
	std::string name;//节点的名称
	int childrenCount;//节点的子节点数量
	std::vector<AssimpNodeData> children;//节点的子节点数组
};

#endif // !ASSIMP_NODE_DATA_H