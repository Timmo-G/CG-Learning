# DAY_2OpenGL-你好三角形

[toc]



- **顶点数组对象**：==Vertex Array Object，VAO==

- **顶点缓冲对象**：==Vertex Buffer Object，VBO==
- **元素缓冲对象**：==Element Buffer Object，EBO== 或 索引缓冲对象==Index Buffer Object，IBO==

管线分为两个部分：

1. 将3D坐标转变为2D坐标
2. 将2D坐标转变为有实际颜色的像素

> 2D坐标与像素不同，2D坐标是点的位置，而像素是该点的近似值，受到屏幕/窗口分辨率的限制

## 图形渲染管线每个阶段

==顶点数据== -> ==顶点着色器== -> 几何着色器 -> 形状装配 -> 光栅化 -> ==片段着色器== -> 测试与混合

> 为了让OpenGL知道我们的坐标和颜色值构成的到底是什么，OpenGL需要你去指定这些数据所表示的渲染类型。我们是希望把这些数据渲染成一系列的点？一系列的三角形？还是仅仅是一个长长的线？做出的这些提示叫做==图元(Primitive)==，任何一个绘制指令的调用都将把图元传递给OpenGL。这是其中的几个：==GL_POINTS、GL_TRIANGLES、GL_LINE_STRIP==。



## 代码逻辑

// 硬编码顶点着色器：将3D坐标传给gpu的正确位置

```c++
const char* vertexShaderSource = "#version 330 core\n"//OpenGL的3.3核心模式
"layout (location = 0) in vec3 aPos;\n"//接收0号属性：一个三维向量
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"// 赋值给内置变量，1.0 是齐次坐标 w
"}\0";
```



// 硬编码片段着色器：运行在显卡上的小程序。决定每个像素最终显示什么颜色。

```c++
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"// 输出变量：最终颜色
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // 绿色\n"// 赋值：绿色 (R, G, B, Alpha)
"}\n\0";
```



 // 初始化 GLFW

```c++
glfwInit();//启动GLFW引擎
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//使用的是3.x版本的OpenGL
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//明确3.3版本
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
```



// 创建窗口：宽，高，标题，是否全屏，是否共享资源

```c++
GLFWwindow* window = glfwCreateWindow(800, 600, "LiteBIM Window - Day 2", NULL, NULL);
```



//报错处理：创建窗口失败

```c++
if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
}

glfwMakeContextCurrent(window);//确定在当前窗口画图
```



//初始化 GLAD ,必须在创建窗口之后进行，因为它要根据窗口环境找显卡函数

```c++
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;//报错处理
}
```



```c++
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
```



//VAO与VBO，将内存里的坐标搬到显存里

```c++
float vertices[] = {
    // 第一个三角形
    -0.5f, -0.5f, 0.0f,  // 左下
     0.5f, -0.5f, 0.0f,  // 右下
    -0.5f,  0.5f, 0.0f,  // 左上
    // 第二个三角形
     0.5f, -0.5f, 0.0f,  // 右下
     0.5f,  0.5f, 0.0f,  // 右上
    -0.5f,  0.5f, 0.0f   // 左上
};

unsigned int VBO, VAO;
glGenVertexArrays(1, &VAO); // 生成说明书ID
glGenBuffers(1, &VBO);      // 生成缓冲ID

glBindVertexArray(VAO);     // 记录接下来的操作

glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定缓冲
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把数据运进缓冲

// 告诉显卡：仓库里的数据，每4个float是一组，对应的位置是0
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0); // 开启0号属性
```



// 渲染循环

```c++
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
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 交换缓冲区
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```





## 画正方形的方法



### 1. 定义六个点

```c++
float vertices[] = {
    // 第一个三角形
    -0.5f, -0.5f, 0.0f,  // 左下
     0.5f, -0.5f, 0.0f,  // 右下
    -0.5f,  0.5f, 0.0f,  // 左上
    // 第二个三角形
     0.5f, -0.5f, 0.0f,  // 右下
     0.5f,  0.5f, 0.0f,  // 右上
    -0.5f,  0.5f, 0.0f   // 左上
};
```



### 2. EBO 索引缓冲 

**修改数据定义：**

```c++
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // 0: 右上
     0.5f, -0.5f, 0.0f,  // 1: 右下
    -0.5f, -0.5f, 0.0f,  // 2: 左下
    -0.5f,  0.5f, 0.0f   // 3: 左上
};

unsigned int indices[] = {
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};
```



**创建 EBO (Element Buffer Object)：**

```c++
unsigned int EBO;
glGenBuffers(1, &EBO);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
```



**修改绘制指令：**

```c++
// 参1: 模式, 参2: 索引数量, 参3: 索引类型, 参4: 偏移量
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```



#### 1.关于EBO和VBO的作用

1. VBO 解决的是 **CPU 到 GPU 的传输效率问题**。它将顶点数据一次性存入显存，减少了每帧渲染时的总线带宽压力。
2. EBO 解决的是 **数据冗余与内存优化问题**。通过将几何拓扑关系（索引）与几何位置数据（坐标）分离，EBO 实现了顶点的复用。在复杂的建筑模型渲染中，使用 EBO 可以大幅降低显存占用，并提升缓存命中率（Vertex Cache），因为 GPU 可以识别出哪些点是已经被处理过的。



#### 2.关于EBO的原理，VAO与VBO,EBO关系

实际上是定点表 + 顺序表；

在VAO已经被绑定的前提下，绑定VBO和EBO；

VAO相当于一个文件夹，里面必须存放VBO和EBO。

### 3.生成 ID 

1. **glGenBuffers (生成 ID)**：
   你走到柜台说：“我要开个户。” 柜员给你一个**账号**（比如 1001）。
   - 此时：你手里只有个号码，账户里没钱，银行也没给你准备具体的保险柜。
2. **glBindBuffer (绑定)**：
   你跟柜员说：“我现在要往 1001 账号里操作。”
   - 此时：银行才真正为你分配了一块空间。
3. **glBufferData (存钱)**：
   你把钱（顶点坐标）递给柜员，存入当前的账户。
