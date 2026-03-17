#define _CRT_SECURE_NO_WARNINGS
//头文件与全局变量
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
 
//创建全局摄像机，方便输入回调函数[3.17更新]
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//设定移动速度[2.25更新]
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
//设定按键映射
void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    //检测按键[3.17更新]
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// 鼠标状态记录[3.1更新]
bool firstMouse = true;    // 用于判断是不是第一次移动鼠标
float yaw = -90.0f;      // 偏航角：初始设为 -90 度，这样一开机就是看向前方 (-Z方向)
float pitch = 0.0f;       // 俯仰角：初始 0 度（平视）
float lastX = 400.0f;     // 鼠标上一次的 X 位置（假设窗口 800x600，初始定在中心）
float lastY = 300.0f;     // 鼠标上一次的 Y 位置
//设置鼠标映射_让物体跟随鼠标移动[3.17更新]
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Y是反的
    lastX = xpos;
    lastY = ypos;

    // 自动计算 cameraFront[3.17更新]
    camera.ProcessMouseMovement(xoffset, yoffset);
}



int main() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//使用的是3.x版本的OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//明确3.3版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//采用“核心模式”
    // 创建窗口：宽，高，标题，是否全屏，是否共享资源
    GLFWwindow* window = glfwCreateWindow(2560, 1440, "Lite Renderer", NULL, NULL);
    //报错处理：创建窗口失败
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);//确定在当前窗口画图
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏鼠标指针，并把它锁在窗口里[3.1更新]
    //处理鼠标输入[3.1更新]
    glfwSetCursorPosCallback(window, mouse_callback);// 注册鼠标位置回调函数，当鼠标移动时，GLFW 自动调用 mouse_callback
    
    // 初始化 GLAD ,必须在创建窗口之后进行，因为要根据窗口环境找显卡函数
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;//报错处理
    }

    Shader ourShader("cube.vs", "cube.fs");
    std::cout << "Shader ID: " << ourShader.ID << std::endl;
 
    float vertices[] = {
        // ---- 位置 ----       ---- 法线 ----
        // 后脸 (Z-)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 0
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 1
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 2
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 3

        // 前脸 (Z+)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 4
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 5
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 6
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 7

        // 左脸 (X-)
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 8
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 9
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 10
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 11

        // 右脸 (X+)
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 12
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 13
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 14
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 15

         // 底脸 (Y-)
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 16
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 17
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // 18
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // 19

         // 顶脸 (Y+)
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 20
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 21
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // 22
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f  // 23
    };
    unsigned int indices[] = {
     0,  1,  2,  2,  3,  0,  // 后
     4,  5,  6,  6,  7,  4,  // 前
     8,  9,  10, 10, 11, 8,  // 左
     12, 13, 14, 14, 15, 12, // 右
     16, 17, 18, 18, 19, 16, // 底
     20, 21, 22, 22, 23, 20  // 顶
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // 记录对数据的描述信息
    glGenBuffers(1, &VBO);      // 显存里的一个大数组
    glGenBuffers(1, &EBO);      //存放索引数据的数组
    //绑定VAO，后续的VBO,EBO绑定和属性配置会被记录
    glBindVertexArray(VAO);     // 记录接下来的操作
    // 把C++内存里的indices数组，复制到显卡的EBO里
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 把C++内存里的vertices数组，复制到显卡的VBO里
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定缓冲
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    // 属性0 (aPos) : 拿3个float，每隔6个float(24字节)拿一次，从第0个位置开始
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // 开启0号属性
    // 属性1 (aNormal) : 拿3个float，每隔6个float拿一次，从第3个float(12字节)的位置开始拿。
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //开启深度测试 
    glEnable(GL_DEPTH_TEST);
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        //每一帧最开始：计算时间增量[2.25更新]
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //处理键盘输入[2.25更新]
        processInput(window, deltaTime);
        //渲染准备
        glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 激活着色器
        ourShader.use();
        //光源位置和颜色
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        //计算 MVP 矩阵
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 2560.0f / 1440.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        //Model矩阵
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        
        //Projection矩阵
        projection = glm::perspective(glm::radians(45.0f), 2560.0f / 1440.0f, 0.1f, 100.0f);
        //传值给Shader
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        // 光照 Uniforms (对应 Light 结构体)
        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // 材质 Uniforms (对应 Material 结构体)
        ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        ourShader.setFloat("material.shininess", 32.0f);

        ourShader.setVec3("viewPos", camera.Position);

        //绘制
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, -1.0f));
        model2 = glm::rotate(model2, (float)glfwGetTime() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));

        ourShader.setMat4("model", model2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}