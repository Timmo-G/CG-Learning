#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // 接收法线

out vec3 FragPos;  // 传给片段着色器的像素位置
out vec3 Normal;   // 传给片段着色器的法线

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // 计算像素在世界空间的位置
    Normal = mat3(transpose(inverse(model))) * aNormal; // 【东大深度：正交规范化矩阵】
    gl_Position = projection * view * vec4(FragPos, 1.0);
}