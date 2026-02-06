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