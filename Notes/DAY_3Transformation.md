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