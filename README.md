# 2026 秋季《计算机体系结构实验》学生仓库

本仓库包含课程实验的学生框架和公开测试。目前包括：

| 编号 | 内容 | 实验说明 | 需要修改的文件 |
| --- | --- | --- | --- |
| Lab 1 | Data Lab：位级运算 | [`labs/lab1-datalab/README.md`](labs/lab1-datalab/README.md) | `bits.c` |
| Lab 2 | Matrix Lab：矩阵乘法优化 | [`labs/lab2-matrix/README.md`](labs/lab2-matrix/README.md) | `src/mygemm.c` |

请分别阅读每个实验目录中的说明，只修改和提交题目指定的文件。不要修改测试程序、
函数接口或构建配置来绕过正确性检查。

## 自动检查

推送代码后，GitHub Actions 会自动评测 Data Lab，并构建 Matrix Lab、运行小规模
正确性测试。相关工作流全部通过，表示当前代码满足公开测试中的正确性要求。最终
成绩以教师使用原始提交文件重新运行的私有评分结果为准。

## 提交方式

每位学生应在教师分配的个人仓库中完成实验，而不是直接修改课程模板仓库。完成
代码后，将提交推送到个人仓库的 `main` 分支即可触发自动评测。
无需向课程模板仓库提交 Pull Request。Pull Request 可以用于个人分支合并或代码
审阅，但不是交作业的必要步骤。

截止时间到达后，教师从每位学生个人仓库的 `main` 分支取得指定提交，并使用私有
评分器重新评测。请在截止时间前确认提交已经推送，而且 GitHub Actions 已运行。

本地运行 Matrix Lab 快速检查：

```bash
cd labs/lab2-matrix
cmake -S . -B build
cmake --build build -j
./build/reg_reuse 6 12 24 48
./build/cache_part3 48 6
./build/cache_part4_o3 48 6
```
