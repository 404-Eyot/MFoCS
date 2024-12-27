# 本原多项式的生成和测试

本原多项式在密码学中有多种应用，特别是在与有限域（Galois Fields）相关的领域。本文件就教学中需要的本原多项式的生成提供一种解决方法：（参考自http://seanerikoconnor.freeservers.com/Mathematics/AbstractAlgebra/PrimitivePolynomials/overview.html，并对其进行了一些更改）

## 使用方法：

### 使用选项

#### C语言

1. Primpoly p n 以参数p n生成一个随机的本原多项式
2. Primpoly -c p n 增加一重本原多项式检验
3. Primpoly -s p n 打印统计信息
4. Primpoly -a p n 以参数p n 生成的所有本原多项式



#### C++

1. Primpoly p n 以参数p n生成一个随机的本原多项式
2. Primpoly -t <Polynomial to test>, p 在F(p)下检验给定多项式是否为本原多项式
3. Primpoly -a p n 以参数p n 生成的所有本原多项式
4. Primpoly -s p n 打印统计信息



#### 注意

由于涉及不同长度参数 需要使用命令行运行！！！
