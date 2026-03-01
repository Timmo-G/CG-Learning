#define _CRT_SECURE_NO_WARNINGS
//头文件与全局变量
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

//顶点着色器：将3D坐标传给gpu的正确位置
const char* vertexShaderSource = "#version 330 core\n"//OpenGL的3.3核心模式
"layout (location = 0) in vec3 aPos;\n"//接收0号属性：一个三维向量
"layout (location = 1) in vec3 aColor;"
"out vec3 ourColor;\n"
"uniform mat4 transform;\n"  // 确保名字是 transform，声明一个 4x4 的统一矩阵[2.6更新]
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   // 核心：必须让坐标乘以矩阵！\n"
"  gl_Position = projection * view * model * vec4(aPos, 1.0f);;\n"// 赋值给内置变量，1.0 是齐次坐标 w[2.6更新]
"   ourColor = aColor;\n"
"}\0";
//片段编译器编码：运行在显卡上的小程序。决定每个像素最终显示什么颜色。
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"// 输出变量：最终颜色
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"// 赋值颜色 (R, G, B, Alpha)
"}\n\0";
//摄像机参数[2.25更新]
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // 摄像机位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 看向前方
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // 头顶向上
//设定移动速度[2.25更新]
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
//设定按键映射
void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 2.5f * deltaTime; // 调整这个系数可以改移动速度
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
// 鼠标状态记录[3.1更新]
bool firstMouse = true;    // 用于判断是不是第一次移动鼠标
float yaw = -90.0f;      // 偏航角：初始设为 -90 度，这样你一开机就是看向前方 (-Z方向)
float pitch = 0.0f;       // 俯仰角：初始 0 度（平视）
float lastX = 400.0f;     // 鼠标上一次的 X 位置（假设窗口 800x600，初始定在中心）
float lastY = 300.0f;     // 鼠标上一次的 Y 位置
//设置鼠标映射_让物体跟随鼠标移动[3.1更新]
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    // 1. 计算鼠标位移
    float xoffset = lastX - xpos;
    float yoffset = ypos - lastY; // Y是反的
    lastX = xpos;
    lastY = ypos;
    // 2. 灵敏度过滤
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    // 3. 更新角度
    yaw += xoffset;
    pitch += yoffset;
    // 4. 约束：防止抬头抬过头把自己翻过去
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    // 5. 将角度转回方向向量 (这是坐标系变换的核心)
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    std::cout << yaw << std::endl;
}
int main() {
    // 初始化 GLFW
    glfwInit();//启动GLFW引擎
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//使用的是3.x版本的OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//明确3.3版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// 4. 采用“核心模式”（去掉老旧功能）
    // 创建窗口：宽，高，标题，是否全屏，是否共享资源
    GLFWwindow* window = glfwCreateWindow(2560, 1440, "LiteBIM Window - Day 5", NULL, NULL);
    //报错处理：创建窗口失败
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);//确定在当前窗口画图
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //[3.1更新]
    //处理鼠标输入[3.1更新]
    glfwSetCursorPosCallback(window, mouse_callback);
    // 初始化 GLAD ,必须在创建窗口之后进行，因为它要根据窗口环境找显卡函数
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;//报错处理
    }
    // 1. 创建并编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 2. 创建并编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 3. 链接成着色器程序对象 
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 4. 删除不需要的着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //VAO与VBO，将内存里的坐标搬到显存里
   float vertices[] = {
     -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // 0: 红
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // 1: 绿
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // 2: 蓝
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // 3: 黄
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, // 4: 紫
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // 5: 青
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // 6: 白
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f  // 7: 黑
    };
    unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,  // 后平面
    4, 5, 6, 6, 7, 4,  // 前平面
    0, 4, 7, 7, 3, 0,  // 左平面
    1, 5, 6, 6, 2, 1,  // 右平面
    0, 1, 5, 5, 4, 0,  // 下平面
    3, 2, 6, 6, 7, 3   // 上平面
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // 生成说明书ID
    glGenBuffers(1, &VBO);      // 生成缓冲ID
    glBindVertexArray(VAO);     // 记录接下来的操作
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定缓冲
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把数据运进缓冲
    // 告诉显卡：数据每3个float是一组，对应的位置是0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // 开启0号属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glEnable(GL_DEPTH_TEST);//开启深度测试 
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // A.每一帧最开始：计算时间增量[2.25更新]
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // B.处理键盘输入[2.25更新]
        processInput(window, deltaTime);
        // C.渲染准备
        glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        // D.计算 MVP 矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);//[2.25更新]

        //Model矩阵
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
      
        //Projection矩阵
        projection = glm::perspective(glm::radians(45.0f), 2560.0f / 1440.0f, 0.1f, 100.0f);
        // E.传值给Shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // F.绘制
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}