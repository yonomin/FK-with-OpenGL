#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; //一个ivec4能储存四个整数
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 200;//模型中所有骨骼的数量上限
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
//每个顶点所用的finalBonesMatrices[MAX_BONES];
//其实只在主程序中被计算并传输了一次，便足够这一帧的所有顶点计算使用，所以其实并没有资源浪费
out vec2 TexCoords;

void main()
{
    vec4 totalPosition = vec4(0.0f);//顶点的最终位置  四次骨骼变换乘上各自权重的最终影响
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)//遍历能影响当前顶点的四个骨骼
    {
        if(boneIds[i] == -1) //如果该骨骼不存在
            continue;//去往下一个骨骼
        if(boneIds[i] >=MAX_BONES) //如果影响这个顶点的骨骼id大于100
        {
            totalPosition = vec4(pos,1.0f);//将totalPosition设置为原始位置
            break;//退出循环
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);//骨骼变化后的位置
        totalPosition += localPosition * weights[i];//顶点位置 加等 这个骨骼的变化的权重
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;//当前骨骼对当前法线的影响
   }
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
	TexCoords = tex;
}