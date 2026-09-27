# 2026 秋季《计算机体系结构实验》学生仓库

本仓库包含课程实验的学生框架和公开测试。目前包括：

| 编号 | 内容 | 实验说明 | 需要修改的文件 |
| --- | --- | --- | --- |
| Lab 1 | Data Lab：位级运算 | [`labs/lab1-datalab/README.md`](labs/lab1-datalab/README.md) | `bits.c` |
| Lab 2 | Matrix Lab：矩阵乘法优化 | [`labs/lab2-matrix/README.md`](labs/lab2-matrix/README.md) | `src/mygemm.c` |

请分别阅读每个实验目录中的说明，只修改和提交题目指定的文件。不要修改测试程序、
函数接口或构建配置来绕过正确性检查。

## 交我算平台运行

在 π 2.0 上运行实验，请阅读 [交我算教程与命令讲义](docs/README.md)。

## 获取代码与自行检查

从教师发布的交大云盘链接下载学生框架，解压后按教程上传到交我算。
在 Slurm 分配的计算节点上编译并运行各 Lab 的公开测试；最终成绩以教师对原始
提交文件重新评测的结果为准。

## 提交方式

每个 Lab 的源码通过对应的交大云盘“源码提交”任务上传。Lab 1 上传 bits.c，
Lab 2 上传 mygemm.c。填写本人学号、姓名，云盘统一命名。报告和日志按课程通知另交。
修改后在同一入口重新上传并替换旧文件，保留本地版本和提交记录。
详细步骤见 [作业提交说明](docs/作业提交说明.md)。

如果在 VS Code 远程窗口修改代码，保存并测试后，将远端最终源码下载到本地再上传。
截止时间以课程通知为准。构建目录、可执行文件和个人 SSH 配置无需提交。

本仓库用于维护和分发学生框架，学生通过云盘交作业，无需提交 GitHub PR。

在计算节点运行 Matrix Lab 快速检查：

```bash
cd labs/lab2-matrix
cmake -S . -B build -DBUILD_TESTING=ON -DLAB_USE_SYSTEM_BLAS=OFF
cmake --build build --parallel 1
(cd build && ctest --output-on-failure)
./build/reg_reuse 6 12 24 48
./build/cache_part3 48 6
for opt in 0 1 2 3; do
  ./build/cache_part4_o${opt} 48 6
done
```
