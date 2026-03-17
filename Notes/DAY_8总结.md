[toc]



# class Camera

## 预处理（头文件保护）

```c++
#ifndef CAMERA_H
#define CAMERA_H
```

防止头文件被重复包含（比如多个 .cpp 文件都 include "Camera.h" 时，避免类 / 枚举重复定义）。

如果 `CAMERA_H` 未定义，就定义它；如果已定义，跳过整个文件内容。

## 头文件

###  GLAD 库

```c++
#include <glad/glad.h>
```

GLAD 是 OpenGL 函数加载器，==负责把显卡驱动的 OpenGL 函数绑定到程序中==，必须在所有 OpenGL 相关头文件前引入。

### GLM

```c++
#include <glm/glm/glm.hpp>                 
#include <glm\glm\gtc/matrix_transform.hpp> 
```

`glm/glm.hpp`：GLM 核心头文件，提供==向量（vec3==）、==矩阵==（mat4）、==数学函数==（radians、normalize 等），是 OpenGL 配套的数学库（纯头文件，无需编译）。

`glm/gtc/matrix_transform.hpp`：GLM 扩展头文件，==提供矩阵变换函数==（比如 `glm::lookAt` 观察矩阵、`glm::perspective` 投影矩阵）

### C++标准向量库

```c++
#include <vector>
```

## 枚举定义

```c++
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
```

**作用**：定义枚举类型 `Camera_Movement`，用「语义化名称」代替数字，提升代码可读性。

**底层值**：默认 `FORWARD=0`、`BACKWARD=1`、`LEFT=2`、`RIGHT=3`，调用时用 `Camera_Movement::FORWARD` 比直接写 0 更易理解。

## 默认常量

```c++
const float YAW = -90.0f; // 初始偏航角：指向 -Z 轴
const float PITCH = 0.0f;  // 初始俯仰角：平视
const float SPEED = 2.5f;  // 移动速度
const float SENSITIVITY = 0.1f;  // 鼠标灵敏度
const float ZOOM = 45.0f; // 视野范围 (FOV)
```

**Yaw（偏航角）**：绕 Y 轴旋转的角度，-90° 让摄像机初始朝向 -Z 轴（OpenGL 世界的 “前方”）。

**Pitch（俯仰角）**：绕 X 轴旋转的角度，0° 表示水平平视（正数向上看，负数向下看）。

**SPEED**：摄像机每秒移动的距离（后续会乘 deltaTime 保证帧率无关）。

**SENSITIVITY**：鼠标移动 1 像素时，摄像机角度变化的系数（越小越灵敏）。

**ZOOM**：透视投影的视野角度（FOV），45° 是常规视角（值越小视野越窄，类似望远镜）。

## Camera 类成员变量

```c++
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
```



### Camera 类构造函数

```c++
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
```

**参数默认值**：

- `position`：默认摄像机在 (0,0,3)（Z 轴正方向，看向原点）。
- `up`：默认世界上方向 (0,1,0)。
- `yaw/pitch`：使用前面定义的默认欧拉角。

**初始化列表**：

- `Front(glm::vec3(0.0f, 0.0f, -1.0f))`：初始朝向 -Z 轴（OpenGL 中，-Z 是 “前方”）。
- `MovementSpeed(SPEED)`：初始化移动速度为默认值。
- `MouseSensitivity(SENSITIVITY)`：初始化鼠标灵敏度。
- `Zoom(45.0f)`：初始化视野角度（你这里重复赋值了，构造函数内又写了 `Zoom=45.0f`，可以删掉一个）。

**函数体**：

- 给 Position/WorldUp/Yaw/Pitch 赋值。
- 调用 `updateCameraVectors()`：根据初始欧拉角，计算 Front/Right/Up 向量（必须初始化，否则视角向量是错的）。

## View矩阵

```c++
glm::mat4 GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}
```

**核心作用**：生成 OpenGL 所需的「观察矩阵（View Matrix）」，将世界坐标系转换为摄像机视角的坐标系。

**glm::lookAt 参数**：

1. `Position`：摄像机位置（眼睛的位置）。
2. `Position + Front`：摄像机看向的目标点（眼睛前方的点）。
3. `Up`：摄像机的上方向（确保画面不会歪）。

在渲染循环中，把这个矩阵传给顶点着色器，作为 View 矩阵。

## 键盘移动逻辑

```c++
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
```

**参数**：

- `direction`：移动方向（枚举值）。
- `deltaTime`：上一帧到当前帧的时间差（保证移动速度不随帧率变化，比如 60 帧和 30 帧移动速度一致）。

**velocity**：实际移动距离 = 基础速度 × 时间差。

**移动逻辑**：

- 前进 / 后退：沿 Front 向量移动（Front 是单位向量，乘 velocity 得到移动距离）。
- 左 / 右：沿 Right 向量移动（左右平移，不是旋转）。

**注意**：这里用 `if` 而非 `else if`，支持同时按多个方向（比如斜着走）。

## 鼠标转动逻辑

```c++
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
```

**参数**：

- `xoffset/yoffset`：鼠标 X/Y 轴的移动偏移量（由鼠标回调函数传入）。
- `constrainPitch`：是否限制俯仰角（默认 true）。

**步骤 1**：偏移量 × 灵敏度，缩小角度变化幅度（避免鼠标动一下视角飞很远）。

**步骤 2**：更新欧拉角：

- X 轴鼠标移动 → 改变 Yaw（左右转）。
- Y 轴鼠标移动 → 改变 Pitch（上下转）。

**步骤 3**：约束俯仰角：限制在 -89° ~ 89°，避免 Pitch=90° 时 Front 向量垂直向上，导致 `updateCameraVectors` 计算出的向量出错（比如 “翻跟头” 后视角颠倒）。

**步骤 4**：调用 `updateCameraVectors`：欧拉角变了，重新计算 Front/Right/Up 向量。

## 更新摄像机向量

```c++
private:
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
```

**核心作用**：将欧拉角（Yaw/Pitch）转换为 3D 方向向量（Front），并推导 Right/Up 向量。

**数学解释**：

1. `glm::radians`：将角度转为弧度（GLM 数学函数只认弧度）。

2. Front 向量计算：

   - x 分量：`cos(Yaw) * cos(Pitch)` → 水平方向的左右分量。
   - y 分量：`sin(Pitch)` → 垂直方向的上下分量。
   - z 分量：`sin(Yaw) * cos(Pitch)` → 水平方向的前后分量。

   

3. `glm::normalize`：将向量归一化（长度变为 1），保证移动速度均匀（不管视角朝向哪里，移动速度都一样）。

**叉乘计算 Right/Up**：

- 叉乘规则：`a × b` 得到垂直于 a 和 b 的向量，符合左手坐标系（OpenGL 标准）。
- `Right = Front × WorldUp`：得到摄像机的右方向（归一化后）。
- `Up = Right × Front`：得到摄像机自身的上方向（不是世界 Up，随视角旋转）。

# main.cpp

## 全局变量

```c++
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
```

创建全局摄像机对象，初始位置在 (0,0,3)（Z 轴正方向，看向原点），全局变量方便输入回调函数访问。

