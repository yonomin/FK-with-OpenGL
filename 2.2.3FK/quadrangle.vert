#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;//set this
uniform mat4 view;//set this  可能没用
uniform mat4 projection;//set this 

uniform vec3 cameraUp;//set this 
uniform vec3 cameraRight;//set this 

void main()
{
	 //gl_Position = projection * view * model * vec4(aPos,0.0, 1.0);
     TexCoords = aTexCoords;
	 // 1. 获取模型的平移（世界坐标位置）
    // Model矩阵的第四列就是平移向量。
    vec3 modelPosition = vec3(model[3]);
    float scaleX = length(vec3(model[0])); // X轴的缩放长度
    float scaleY = length(vec3(model[1])); // Y轴的缩放长度
    vec2 scaledPos = aPos * vec2(scaleX, scaleY);
    // 2. 根据 aPos (四边形顶点) 和摄像机方向计算世界坐标
    // 目标：将 2D 顶点 (aPos) 扩展到 3D 空间，并使其坐标系与摄像机的 (Right, Up) 对齐。
    // modelPosition 确定了四边形的中心。
    // aPos.x 乘以 cameraRight 向量，使其沿摄像机 X 轴方向平移。
    // aPos.y 乘以 cameraUp 向量，使其沿摄像机 Y 轴方向平移。
    vec3 worldPosition = modelPosition + (cameraRight * scaledPos.x) + (cameraUp * scaledPos.y);

    // 3. 最终变换
    // 将计算出的世界坐标提升为 vec4，w 设为 1.0
    gl_Position = projection * view * vec4(worldPosition, 1.0);
}