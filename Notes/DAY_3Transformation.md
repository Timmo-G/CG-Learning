# DAY_3_Transformation

[toc]

## 2D transformation

### 1. scale matrix

#### 均匀缩放：


$$
\begin{matrix}
 s & 0 \\
 0 & s  \\

\end{matrix}
$$

#### 不均匀缩放：

$$
\begin{matrix} 
m & 0  \\
0 & s  \\ 

\end{matrix}
$$

#### 切变：

$$
\begin{matrix}
 1 & a \\
 0 & 1 \\
\end{matrix}
$$

$$
\begin{matrix}



\end{matrix}
$$

#### 旋转的矩阵：

$$
\begin{matrix}
cosx & -sinx\\
sinx & cosx\\
\end{matrix}
$$

#### 总结：

<center>x' = M x<center/>



### 2. 齐次坐标

 

#### 用**齐次坐标**表示**仿射变换**：

$$
\begin{matrix}
a & b & tx\\
c & d & ty\\
0 & 0 & 1

\end{matrix}
$$

#### **逆变换**：



# DAY_3glm库实现图形的旋转

## glm代码逻辑

- **glm::mat4 trans = glm::mat4(1.0f);**
  - 
  - **作用**：创建一个 4x4 的 **单位矩阵**。
  - **逻辑**：在矩阵运算中，单位矩阵相当于数字里的 **“1”**。任何向量乘以它都保持不变。我们把它作为“起始状态”。
- **glm::rotate(trans, time, axis);****作用**：在原有矩阵的基础上，增加一个“旋转”逻辑。**逻辑**：它并不直接改动顶点，而是生成一个能让顶点旋转的**数学逻辑**。
- **glm::scale(trans, size);****作用**：在原有矩阵基础上，增加“缩放”逻辑。
- **glm::value_ptr(trans)****作用**：将 GLM 复杂的矩阵对象，转换成 OpenGL 显卡能听懂的 “简单浮点数数组地址”**。

## 代码的整体前后逻辑

**初始化阶段：**

1. 创建VAO->创建VBO->编译shader
2. 将坐标数据保存进入VBO中
3. 循环渲染

## while中的代码逻辑

1. **processInput（获取指令）**：
   - 看看用户有没有点关闭，或者按了键盘。
2. **glClearColor & glClear（擦黑板）**：**必须做！** 如果不擦，上一帧的王冠会留在屏幕上，最后糊成一团。
3. **glUseProgram（点将）**：告诉 GPU：“我们要用这组工人（着色器）来干活”。
4. **glm::rotate / scale（大脑思考）**：CPU 计算当前这一帧，王冠应该处于什么角度、什么大小。
5. **glUniformMatrix4fv（下达命令）**：CPU 把算好的矩阵发给 GPU。**关键点**：这步完成了 CPU 到 GPU 的数据传递。
6. **glBindVertexArray & glDrawElements（真正动手）**：GPU 根据说明书（VAO）从仓库取数，乘以收到的矩阵，画出形状。
7. **glfwSwapBuffers（展示成果）**：把画好的这幅画推到前台给用户看。



#### **在项目中，为什么要引入矩阵库，而不是手动修改顶点坐标？**

“手动修改 VBO 中的顶点坐标（CPU逐个计算）会带来巨大的性能开销，因为这涉及到频繁的内存与显存之间的大数据量交换（Bus Bandwidth）。
而引入 **变换矩阵** 结合 **Uniform 变量**，我只需要向 GPU 发送 16 个浮点数（4x4矩阵）。GPU 利用其强大的并行计算能力，在顶点着色器中一次性完成成千上万个顶点的空间变换。这体现了图形学中**‘计算量留在 GPU，数据传输最小化’**的优化原则。”

#### 今日代码逻辑

第一行：

```
glm::mat4 trans = glm::mat4(1.0f);
```

这行做的事情只有一个：

> **创建一个 4×4 的单位矩阵**

为什么要从单位矩阵开始？

因为矩阵变换是“叠加”的：

- 单位矩阵 = 什么都不做
- 后面的 rotate / scale 都是“在它基础上修改”

如果你不是从单位矩阵开始，后面所有变换都会乱。

------

第二行（非常关键）：

```
trans = glm::rotate(
    trans,
    (float)glfwGetTime(),
    glm::vec3(0.0f, 0.0f, 1.0f)
);
```

我们先整体说一句人话版：

> **在已有的 trans 矩阵基础上，绕 Z 轴旋转一个角度**

现在拆参数。

### 1️⃣ 第一个参数：`trans`

```
glm::rotate(原矩阵, ...)
```

意思是：

> **不是“生成一个新的旋转矩阵”，
>  而是“在原矩阵上追加一个旋转”**

GLM 的 rotate 本质是：

```
return 原矩阵 * 旋转矩阵;
```

这就是为什么 OpenGL/GLM 是“状态叠加思维”。

------

### 2️⃣ 第二个参数：`(float)glfwGetTime()`

```
glfwGetTime()
```

返回的是：

> 程序运行到现在的时间（秒）

类型是 `double`，所以你看到：

```
(float)
```

这是一个**显式类型转换**，因为 GLM 的 rotate 需要 `float`。

**意义**：

- 时间一直在变
- 角度一直在变
- 物体就会不停旋转

------

### 3️⃣ 第三个参数：`glm::vec3(0.0f, 0.0f, 1.0f)`

这是：

> **旋转轴**

也就是：

- X 轴：(1, 0, 0)
- Y 轴：(0, 1, 0)
- Z 轴：(0, 0, 1)

这里用 Z 轴，表示：

> 在屏幕平面内转圈（二维旋转）

------

第三行：

```
trans = glm::scale(
    trans,
    glm::vec3(0.5f, 0.5f, 0.5f)
);
```

整体人话：

> **在已有变换基础上，再缩小到一半**

参数解释：

### 1️⃣ 第一个参数：`trans`

同样是：

> 在“当前状态”上继续叠加

------

### 2️⃣ 第二个参数：`glm::vec3(0.5f, 0.5f, 0.5f)`

表示：

- x 缩放 0.5
- y 缩放 0.5
- z 缩放 0.5

也就是：

> 整体缩小一半

------

⚠️ **非常重要的隐含点（很多人第一次都不懂）**

```
rotate → scale
```

不是“先旋转再缩放”这么简单。

在矩阵里，真实顺序是：

> **scale 先作用，rotate 后作用**

因为矩阵乘法是 **右乘先发生**。

这也是为什么 OpenGL 初学者会觉得“反直觉”。

------

第四行：

```
unsigned int transformLoc =
    glGetUniformLocation(shaderProgram, "transform");
```

这一行开始，**你从 CPU 世界进入 GPU 世界了**。

它的意思是：

> **问 OpenGL：shader 里叫做 "transform" 的 uniform 变量，现在在哪？**

解释参数：

### `shaderProgram`

- 当前使用的 shader 程序
- 里面包含 vertex shader + fragment shader

### `"transform"`

这是 **shader 代码里的名字**，比如：

```
uniform mat4 transform;
```

⚠️ 注意：

- 这是字符串
- 拼错一个字母，位置就是 `-1`

------

第五行（整个过程的终点）：

```
glUniformMatrix4fv(
    transformLoc,
    1,
    GL_FALSE,
    glm::value_ptr(trans)
);
```

一句人话翻译：

> **把 CPU 里的 trans 矩阵，传给 GPU 里的 uniform mat4 transform**

现在逐个参数解释。

------

### 1️⃣ `transformLoc`

这是刚刚拿到的：

> uniform 的“地址 / 位置”

OpenGL 不认名字，只认位置。

------

### 2️⃣ `1`

表示：

> 我要传 **1 个矩阵**

如果你传数组，这里可以是 10、100。

------

### 3️⃣ `GL_FALSE`

意思是：

> **不要转置矩阵**

原因：

- GLM 默认是 **列主序**
- OpenGL 期望的也是 **列主序**
- 所以不需要转置

如果你这里写 `GL_TRUE`，画面会直接乱。

------

### 4️⃣ `glm::value_ptr(trans)`

这是一个非常 C++ 的点。

它的意思是：

> **拿到 trans 这个 mat4 在内存中的首地址**

因为：

- OpenGL 是 C API
- 只接受 `float*`
- 不懂什么 `glm::mat4`

`value_ptr` 的作用就是：

> 把 C++ 对象 → 裸指针

------

## 把整段代码压缩成一个“状态模型”

你现在这段代码，本质是：

1. 在 CPU 上计算一个变换矩阵（GLM）
2. 每一帧更新它（时间驱动）
3. 把它传进 GPU（uniform）
4. GPU 用它去变换顶点