#ifndef MESH_H
#define MESH_H
#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>

using namespace std;

//能影响这个顶点的最多骨骼数
#define MAX_BONE_INFLUENCE 4
//一个顶点的结构 包含其若干属性
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];//能影响这个顶点的骨骼ID
    float m_Weights[MAX_BONE_INFLUENCE];//影响着这个顶点的骨骼的权重
};
//一个材质的结构 包含其若干属性
struct Texture {
    unsigned int id;
    string type;
    string path;
};
//一个网格类 包含其若干属性与函数
class Mesh {
public:
    //一个网格的公共成员变量
    vector<Vertex>       vertices;//一个网格中的所有顶点
    vector<unsigned int> indices;//一个网格的所有Indices
    vector<Texture>      textures;//一个网格的所有材质
    unsigned int VAO;//该网格的VAO

    //网格的构造函数；储存vertices，indices，textures；
    //调用构造网格的函数setupMesh();
    //最终构造出一个网格
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        setupMesh();
    }

    //绘制当前网格;为网格的材质组中的材质逐个激活TEXTURE UNIT，找到各自在sampler2D的对应名称，并绑定到TEXTURE UNIT，为当前TEXTURE UNIT激活当前材质;
    //最终按照VAO和EBO绘制整个当前网格
    void Draw(Shader &shader)
    {
        //统计当前网格的各类材质数量
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)//遍历当前网格的材质组
        {
            glActiveTexture(GL_TEXTURE0 + i); //按照遍历的数量启用TEXTURE UNIT
            string number;//统一材质类型的数量，要对应上shader中材质sampler2D的序号部分;
            string name = textures[i].type;//材质名称，要对应上shader中材质sampler2D的名称部分;按照被遍历到的材质的类型赋值
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string
            
            //以材质名和材质序号组合选择shader中对应的sampler2D名称，并将sampler2D名称对应到当前材质的TEXTURE UNIT
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            //将当前材质组中的材质应应用到TEXTURE UNIT
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // 绘制
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 重置TEXTURE UNIT到初始状态
        glActiveTexture(GL_TEXTURE0);
    }
private:
    //一个网格的私有成员变量
    unsigned int VBO, EBO;//该网格的VBO和EBO

    //构造网格的函数;被网格构造方法Mesh()调用;依照各类成员函数填充了VBO，EBO;
    //最终对当前网格的VAO指定了VBO和EBO，填充了VBO和EBO，指定了7个AttribPointer
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif // !MESH_H
