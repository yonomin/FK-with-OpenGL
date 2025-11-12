#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "assimp_glm_helpers.h"
#include "boneInfo.h"
#include "assimpNodeData.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


using namespace std;
//模型类  类成员变量:
//vector<Texture> textures_loaded;//读取过的材质组成的列表
//vector<Mesh>    meshes;//一个模型的所有网格
//string directory;//当前模型的文件目录
//bool gammaCorrection;//当前模型是否启用gamma矫正
//std::map<string, BoneInfo> m_BoneInfoMap;//骨骼信息map;包含名称，BoneInfo(包含id,offset)
//int m_BoneCounter = 0;//模型的m_BoneCounter会比BoneInfo.id大1
class Model
{
public:
	//一个模型的共有成员变量
	vector<Texture> textures_loaded;//读取过的材质组成的列表
	vector<Mesh>    meshes;//一个模型的所有网格
	string directory;//当前模型的文件目录
	bool gammaCorrection;//当前模型是否启用gamma矫正

	//一个模型的构造函数;
	//调用导入模型私有函数loadModel(path);
	//最终构造出一个模型;
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	//绘制函数
	//调用模型的所有网格，并调用逐个网格的draw函数
	void Draw(Shader &shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	//返回模型的m_BoneInfoMap的函数
	auto &GetBoneInfoMap() { return m_BoneInfoMap; }//[DAMN]

	//返回模型的m_BoneCounter的函数
	int &GetBoneCount() { return m_BoneCounter; }

	AssimpNodeData GetRootNode() { return m_RootNode; }

	vector<glm::mat4> GetTransformations() { return m_Transformations; }
	vector<std::string> GetBoneIDtoNames() { return m_BoneIDtoNames; }

private:
	//一个模型的私有成员变量
	std::map<string, BoneInfo> m_BoneInfoMap;//骨骼信息map;包含名称，BoneInfo(包含id,offset)
	int m_BoneCounter = 0;//当前模型的骨骼数量
	AssimpNodeData m_RootNode;//当前模型的骨骼节点结构

	vector<glm::mat4> m_Transformations;//每个骨骼的transformation,索引是id,这个变量是即时用的
	vector<std::string>m_BoneIDtoNames;//每个骨骼的名称，索引是id

	//导入模型的函数;
	//通过path构造出scene;path被提取出文件目录，存入公共成员变量directory;
	//调用处理节点的私有函数processNode(scene->mRootNode, scene);
	void loadModel(string const &path)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
		m_Transformations.resize(m_BoneCounter);//processNode结束m_BoneCounter应该就被赋值完了，这时候拿来初始化m_Transformation数组
		m_BoneIDtoNames.resize(m_BoneCounter);
		ReadHierarchyData(m_RootNode, scene->mRootNode);
	}

	//处理节点的函数;递归处理所有节点,通过processMesh(mesh, scene)处理节点中的网格;
	//最终把所有网格保存到公共成员变量meshes;
	void processNode(aiNode *node, const aiScene *scene)
	{

		for (unsigned int i = 0; i < node->mNumMeshes; i++)//遍历节点中所有Meshes
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void ReadHierarchyData(AssimpNodeData &dest, const aiNode *src)
	{
		assert(src);

		dest.name = src->mName.data;//将aiNode的mName存入一个AssimpNodeData的name变量中
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);////将aiNode的mTransformation存入一个AssimpNodeData的transformation变量中
		dest.childrenCount = src->mNumChildren;////将aiNode的子节点数mNumChildren存入一个AssimpNodeData的子节点数变量中
		
		auto it = m_BoneInfoMap.find(dest.name);//如果节点名是骨骼名（存在于m_BoneInfoMap中），返回迭代器
		if (it != m_BoneInfoMap.end())//如果迭代器不为end则当前node是骨骼
		{
			m_Transformations[it->second.id] = dest.transformation;
			m_BoneIDtoNames[it->second.id] = dest.name;
		}
		
		for (int i = 0; i < src->mNumChildren; i++)//遍历这个aiNode的子节点
		{
			AssimpNodeData newData;//创建一个新的AssimpNodeData
			ReadHierarchyData(newData, src->mChildren[i]);//递归操作每一个子节点
			dest.children.push_back(newData);//填充进循环外的节点的children变量中
		}
	}

	//处理assimp网格的函数
	//最终返回一个自己定义的网格结构
	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		//三个函数内变量
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)//遍历mesh中所有顶点
		{
			Vertex vertex;
			SetVertexBoneDataToDefault(vertex);
			vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);//读取当前顶点位置
			vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);//读取法向

			if (mesh->mTextureCoords[0])//
			{
				glm::vec2 vec{};
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;//读取属于第i个顶点的索引为0的贴图坐标
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);//没有贴图的情况

			vertices.push_back(vertex);//保存这个顶点到自建变量
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)//遍历网格中每个面
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)//遍历面中的每一个Indices
				indices.push_back(face.mIndices[j]);//收集Indices
		}
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		//读到这里
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());//从

		ExtractBoneWeightForVertices(vertices, mesh);

		return Mesh(vertices, indices, textures);
	}

	//将影响当前顶点的每个骨骼的属性设为默认值的函数
	//最终每一个顶点的m_BoneIDs属性设为-1，m_Weights属性设为0.0f
	void SetVertexBoneDataToDefault(Vertex &vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.m_BoneIDs[i] = -1;
			vertex.m_Weights[i] = 0.0f;
		}
	}

	//加载assimp材质的函数
	//将加载过的贴图放入textures_loaded，返回一个贴图列表
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
			}
		}
		return textures;
	}

	//从文件中加载材质
	//最终返回被加载的材质的id
	unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false)
	{
		string filename = string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	//从aiMesh中读取骨骼
	//填充m_BoneInfoMap，填充m_BoneCounter，填充当前顶点的boneID, weight
	void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh)
	{
		auto &boneInfoMap = m_BoneInfoMap;//拿出骨骼列表
		int &boneCount = m_BoneCounter;//拿出骨骼数

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)//遍历网格aimesh中所有骨骼
		{
			int boneID = -1;//初始化骨骼id
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();//从aimesh中获取当前骨骼的骨骼名称
			if (boneInfoMap.find(boneName) == boneInfoMap.end())//如果boneInfoMap中没有这个骨骼名称
			{
				BoneInfo newBoneInfo{};//新建BoneInfo结构对象
				newBoneInfo.id = boneCount;//将BoneCount骨骼数量作为id传入newBoneInfo
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);//将当前骨骼的偏移矩阵传入
				boneInfoMap[boneName] = newBoneInfo;//对骨骼名称连上BoneInfo
				boneID = boneCount;
				boneCount++;//模型的boneCount会比newBoneInfo.id大1
			}
			else
			{
				boneID = boneInfoMap[boneName].id;//有的话
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;//读取网格中当前骨骼的权重列表
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;//读取网格中的当前骨骼影响了多少个顶点

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)//遍历被骨骼影响的所有顶点
			{
				int vertexId = weights[weightIndex].mVertexId;//被当前骨骼影响的顶点id(?网格中每个mVertexId应该都是独一无二的)
				float weight = weights[weightIndex].mWeight;//当前骨骼对这个顶点的权重影响
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}

	//设置当前顶点的骨骼属性
	//最终将boneID和weight赋值给当前顶点第[i]个骨骼属性的m_BoneIDs与m_Weights
	void SetVertexBoneData(Vertex &vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)//遍历影响这个顶点的骨骼
		{
			if (vertex.m_BoneIDs[i] < 0)//如果当前骨骼ID为初始值-1
			{
				vertex.m_Weights[i] = weight;//给当前顶点的当前骨骼权重
				vertex.m_BoneIDs[i] = boneID;//给当前顶点的当前骨骼id
				break;
			}
		}
	}
};

#endif // !MODEL_H
