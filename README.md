# 2026 秋季《计算机体系结构实验》学生仓库

本仓库包含课程实验的学生框架和公开测试。目前包括：

| 编号 | 内容 | 实验说明 | 需要修改的文件 |
| --- | --- | --- | --- |
| Lab 1 | Data Lab：位级运算 | [`labs/lab1-datalab/README.md`](labs/lab1-datalab/README.md) | `bits.c` |
| Lab 2 | Matrix Lab：矩阵乘法优化 | [`labs/lab2-matrix/README.md`](labs/lab2-matrix/README.md) | `src/mygemm.c` |

请分别阅读每个实验目录中的说明，只修改和提交题目指定的文件。不要修改测试程序、
函数接口或构建配置来绕过正确性检查。

## 自动检查

推送代码后，GitHub Actions 会自动构建实验框架。Matrix Lab 还会运行小规模正确性
测试；`Lab 2 correctness` 工作流全部通过，表示当前代码满足必做部分的正确性
要求。最终成绩以教师使用原始提交文件重新运行的评分结果为准。

本地运行 Matrix Lab 快速检查：

```bash
cd labs/lab2-matrix
cmake -S . -B build
cmake --build build -j
./build/reg_reuse 6 12 24 48
./build/cache_part3 48 6
./build/cache_part4_o3 48 6
```
