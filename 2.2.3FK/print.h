#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <glm/glm.hpp>
#include <map>

class PrintGLM
{
public:
    static void Mat4VectorPrint(const std::vector<glm::mat4> &Mat4Vector)
    {
        for (const auto &mat : Mat4Vector)
        {
            Mat4Print(mat);
        }
    }

    static void BoneInfoMapPrint(const std::map<std::string, BoneInfo> &m_BoneInfoMap) 
    {
        for (const auto &pair : m_BoneInfoMap)
        {
            const std::string &boneName = pair.first;
            const BoneInfo &info = pair.second;

            std::cout << "Bone Name (Key): " << boneName
                << ", Bone ID (Value): " << info.id 
                << std::endl;
            Mat4Print(info.offset);
        }
    }

    static void Mat4Print(const glm::mat4 &mat)
    {
        std::cout << "Matrix 4x4:\n";
        for (int i = 0; i < 4; ++i) // 行
        {
            for (int j = 0; j < 4; ++j) // 列
            {
                // 访问元素：mat[j][i]
                std::cout << mat[j][i] << "\t";
            }
            std::cout << "\n";
        }
    }
private:



};
#endif // !PRINT_H
