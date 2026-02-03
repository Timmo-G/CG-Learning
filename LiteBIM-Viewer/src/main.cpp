#define _CRT_SECURE_NO_WARNINGS
//头文件与全局变量
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//顶点着色器：将3D坐标传给gpu的正确位置
const char* vertexShaderSource = "#version 330 core\n"//OpenGL的3.3核心模式
"layout (location = 0) in vec3 aPos;\n"//接收0号属性：一个三维向量
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"// 赋值给内置变量，1.0 是齐次坐标 w
"}\0";
//片段编译器编码：// 运行在显卡上的小程序。决定每个像素最终显示什么颜色。
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"// 输出变量：最终颜色
"void main()\n"
"{\n"
"   FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f); // 绿色\n"// 赋值：绿色 (R, G, B, Alpha)
"}\n\0";

int main() {

    // 初始化 GLFW
    glfwInit();//启动GLFW引擎
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//使用的是3.x版本的OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//明确3.3版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// 4. 采用“核心模式”（去掉老旧功能）

    // 创建窗口：宽，高，标题，是否全屏，是否共享资源
    GLFWwindow* window = glfwCreateWindow(800, 600, "LiteBIM Window - Day 2", NULL, NULL);
    //报错处理：创建窗口失败
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);//确定在当前窗口画图

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

    // 4. 删除不需要的着色器（已经链接好了）
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    //VAO与VBO，将内存里的坐标搬到显存里
    float vertices[] = {
     0.5f,  0.5f, 0.0f,  // 0: 右上
     0.5f, -0.5f, 0.0f,  // 1: 右下
    -0.5f, -0.5f, 0.0f,  // 2: 左下
    -0.5f,  0.5f, 0.0f,  // 3: 左上
     0.0f,  0.5f, 0.0f   // 4: 中上
    };

    unsigned int indices[] = {
    0, 1, 2, // 第一个三角形
    1, 2, 3, // 第二个三角形
    2, 4, 1  // 第三个三角形
    };

    unsigned int EBO;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // 生成说明书ID
    glGenBuffers(1, &VBO);      // 生成缓冲ID

    glBindVertexArray(VAO);     // 记录接下来的操作

    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定缓冲
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把数据运进缓冲

    // 告诉显卡：仓库里的数据，每3个float是一组，对应的位置是0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // 开启0号属性

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 输入检测：是否接入了esc
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // 渲染指令：把屏幕清空为深蓝色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 激活着色器程序
        glUseProgram(shaderProgram);

        // 绑定我们要用的说明书 (VAO)
        glBindVertexArray(VAO);

        // 真正画三角形的指令：从第0个点开始，画3个点
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // 交换缓冲区
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}