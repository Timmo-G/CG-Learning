# Day_1_GAMES101

[toc]



- 图形学依赖于线性代数 微积分 统计
- 基础物理
- 信号处理 数值分析

## 图形学API

- ==OPENGL== 
- ==DIRECTX== 
- ==VULCAN==

## 线性代数

### 1. 向量

方向

长度

1. 向量点乘能看到是否接近，是否同向（例如高光，镜面反射的时候，需要看这个向量是否接近你的观测点）
2. 向量叉积产生垂直于两个向量的新向量，由右手螺旋定则确定方向
3. 叉积可以用来判断向量的左右和内外（三角形光栅化，光线追踪）

### 2. 矩阵

1. 矩阵的旋转（摄像机的扭转）



# DAY_1_OPENGL

## 环境配置

- GLFW配置时需要一个**include（包含目录）**，本质是一个说明书；之后添加**依赖项（Linker->input)**，给了VS原材料.lib文件，否则链接会报错。

- GLAD.c文件放入src文件夹，代码也存入src文件夹。（之所以不是.lib文件是因为glad需要去显卡驱动里找函数地址，每个人的OpenGL的版本不同，GLAD根据在线勾选的配置实时生成一段代码。

- main.cpp文件是入口，这是操作系统运行程序的时候的第一行，写明什么时候开窗等等。
  - 翻译：编译器把main.cpp变成main.obj把glad.c变成glad.obj；
  - 组装：链接器把main.obj和glad.obj链接，并去glfw中寻找到对应代码后缝合起来
  - 交付：最终形成了exe文件

- 这是为了编译效率和模块化管理的考虑，.lib库可以极大缩短时间，使用glad.c可以更灵活。

## 源文件代码逻辑

1. 初始化和配置

```c++
glfwIint();//初始化启用插件
glfwwWindowHint(...);//告诉GLFW，我们需要使用的是OpenGL的3.3版本
```

2. 创建窗口

```c++
GLFWwindow* window = glfwCreateWindow(...);//向Windows申请一个800x 600的窗口
glfwMakeContextCurrent(window);//接下来所有的操作都在这个窗口内
```

3. 加载GLAD

```c++
galdLoadGLLoader(...);//GLAD去找显卡驱动里面的函数地址
```

4. 渲染循环

```c++
while(!glfwWindowShouldClose(window)){
    processInput(window);//查看用户有没有按esc
    glClearColor(...);//选择颜色
    glClear(...);//刷成这个颜色
    glfwSwapBuffers(window);//交换缓冲区
    glfwPollEvebts();//处理窗口消息
}
```

> 双缓冲

glfwSwapBuffers为图形学的核心

前缓冲区：显示画面

后缓冲区：绘制画面

SwapBuffers：当后台画好了之后对调缓冲区

> 在Revit中，旋转视角就是后台的==渲染循环==在工作
>
> 每次旋转鼠标就是GLFW获取了鼠标位移，计算出了新的相机坐标，==OpenGL==指令让显卡重新绘制，==GLAD==确保这些指令发送给了正确的驱动地址，最后==SwapBuffers==让用户看到了流畅的旋转动画

- GLFW是一个窗口和输入管理器，像操作系统创建窗口，设置分辨率，处理输入，上下文管理
- GLAD是一个函数加载器，OpenGL位于显卡驱动，且不同显卡的内存地址不同
- lib文件（库文件）相当于**预制好的构件**。是**Static Library（静态库）**的后缀。
  - 在Windows下，.lib有两种不同的用法：
    1. 静态库：代码全在这个.lib中，链接时内容会直接拷贝进.exe文件  
    2. 导入库：代码本体在.dll（动态链接库）里，链接时VS只记录下某个函数在某个dll文件中。
