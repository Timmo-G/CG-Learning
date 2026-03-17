
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>                 
#include <glm/glm/gtc/matrix_transform.hpp> 
#include <vector>

// 定义移动方向，增加代码可读性
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 默认摄像机参数
const float YAW = -90.0f; // 初始偏航角：指向 -Z 轴
const float PITCH = 0.0f;  // 初始俯仰角：平视
const float SPEED = 2.5f;  // 移动速度
const float SENSITIVITY = 0.1f;  // 鼠标灵敏度
const float ZOOM = 45.0f;// 视野范围 (FOV)

class Camera
{
public:
    float Zoom; 
    // 摄像机属性
    glm::vec3 Position;  // 摄像机位置
    glm::vec3 Front;     // 摄像机朝向（前）
    glm::vec3 Up;        // 摄像机上方
    glm::vec3 Right;     // 摄像机右方
    glm::vec3 WorldUp;   // 世界的上方方向 (0,1,0)

    // 欧拉角（控制视角的关键）
    float Yaw;
    float Pitch;
    
    // 选项
    float MovementSpeed;
    float MouseSensitivity;

    // 构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(45.0f)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Zoom = 45.0f;
        updateCameraVectors(); // 初始化计算三个轴向
    }

    // 获取观察矩阵 (View Matrix)
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 键盘移动逻辑
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // 鼠标转动逻辑
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // 约束俯仰角，防止“翻跟头”
        if (constrainPitch) {
            if (Pitch > 89.0f)  Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        updateCameraVectors(); // 角度变了，重新计算方向向量
    }

private:
    // 计算 Front, Right, Up 向量的数学函数
    void updateCameraVectors() {
        // --- 核心数学：将欧拉角转为 3D 向量 (GAMES101 考点) ---
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front); // 归一化方向向量
        // 叉乘：右向量 = 朝向 × 世界向上
        Right = glm::normalize(glm::cross(Front, WorldUp));
        // 叉乘：摄像机向上 = 右向量 × 朝向
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif