# Lab 2：矩阵乘法性能优化实验

## 1. 实验任务


你需要在框架的 `src/mygemm.c` 中实现多种矩阵乘法算法，依次完成：

1. 基础矩阵乘法和寄存器复用；
2. 2×2 或更大的寄存器分块；
3. 六种循环次序；
4. 六种缓存分块实现；
5. 缓存分块与寄存器分块结合的综合优化；
6. 正确性验证、运行时间测量和实验数据收集。

所有函数计算：

```text
C = C + A * B
```

`A`、`B`、`C` 均为 `n × n` 的 `double` 方阵，使用一维数组按行优先存储：

```c
A[i][j]  <=>  A[i * n + j]
```

本实验在自己的 Linux 环境中完成。

## 2. 需要提交的内容

提交：

1. 完成后的 `src/mygemm.c`；
2. 实验数据文件或实验报告，包含本说明第 9 节要求的表格和图；
3. 如课程平台要求，再提交运行日志目录 `results/`。
4. 如完成 Bonus，提交对应源码、补丁、AI agent 使用记录和 Bonus 数据。

不要提交上述未提及的任何其他文件

## 3. Linux 环境准备

### 3.1 安装编译工具

需要：

- 64 位 Linux；
- GCC 或 Clang；
- CMake 3.16 或更高版本；
- Make 或 Ninja；
- Git；
- 建议至少 8 GiB 内存。

Ubuntu/Debian 可运行：

```bash
sudo apt update
sudo apt install build-essential cmake git
```

### 3.2 不需要安装 BLAS

框架已经内置本实验所需的跨平台 `cblas_dgemm` 参考实现。不需要安装 Intel MKL、OpenBLAS 或其他数学库，也不需要配置库路径。

配置 CMake 时应看到：

```text
Using bundled portable cblas_dgemm reference backend
```

如果出现 `/act/opt/intel/mkl`、`mkl_avx2`、`sbatch` 或 Tardis 相关错误，说明使用了旧框架，请重新下载课程提供的版本。

## 4. 获取和检查框架

请从Canvas上下载实验所需的代码框架。

主要文件：

| 文件 | 作用 | 是否需要修改 |
|---|---|---|
| `src/mygemm.c` | 学生实现所有矩阵乘法函数 | 是 |
| `include/mygemm.h` | 函数接口 | 否 |
| `benchmarks/reg_reuse.c` | 寄存器复用测试程序 | 否 |
| `benchmarks/cache_part3.c` | 循环次序与缓存分块测试程序 | 否 |
| `benchmarks/cache_part4.c` | 综合优化测试程序 | 否 |
| `src/util.c`、`include/util.h` | 初始化、计时和正确性检查 | 否 |
| `third_party/labblas/` | 内置参考 BLAS | 否 |
| `scripts/run_all.sh` | 批量运行脚本 | 否 |

评分时只考虑你的 `src/mygemm.c`，因此不要依赖对其他源文件的修改。

## 5. 编译框架

从课程仓库根目录进入 Lab 2 学生框架，然后运行：

```bash
cd labs/lab2-matrix
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

`ctest` 应显示两个测试通过：

```text
bundled_blas_reference
framework_correctness_pipeline
```

这两个测试只检查框架和参考 BLAS，不检查尚未实现的 `src/mygemm.c`。

如果修改 `src/mygemm.c` 后需要重新编译，只需运行：

```bash
cmake --build build -j
```

建议先保留编译器警告。不得通过删除正确性检查或修改测试程序来消除错误。

## 6. 编程任务

所有函数都在 `src/mygemm.c` 中实现。不得修改函数名称、参数或返回类型。

实现中不得调用 MKL、OpenBLAS、BLAS、Eigen 或其他现成矩阵乘法函数。框架中的 `cblas_dgemm` 只用于生成参考答案，不计入实验结果中的运行时间。

### 6.1 `dgemm0`：基础三重循环

实现最直接的 `ijk` 三重循环版本：

```c
void dgemm0(const double *A, const double *B, double *C, int n);
```

要求：

- 每次内层循环直接更新 `C[i*n+j]`；
- 结果必须是 `C = C + A * B`；
- 不允许假设 `C` 初始为零。

### 6.2 `dgemm1`：单元素寄存器复用

```c
void dgemm1(const double *A, const double *B, double *C, int n);
```

要求：

- 在局部 `double` 变量中保存 `C[i*n+j]`；
- 完成整个 `k` 循环后再写回 `C`；
- 与 `dgemm0` 使用相同的计算语义。

### 6.3 `dgemm2`：2×2 寄存器分块

```c
void dgemm2(const double *A, const double *B, double *C, int n);
```

要求：

- 每次处理一个 2×2 的 `C` 子块；
- 使用 4 个局部累加变量保存该子块；
- 在内层循环中复用读取的 `A` 和 `B` 元素；
- 内层循环完成后再写回 4 个结果。

正式测试规模n均为偶数。推荐额外处理奇数规模的边界，但不要为了处理边界破坏偶数规模下的性能。

### 6.4 `dgemm3`：更积极的寄存器分块

```c
void dgemm3(const double *A, const double *B, double *C, int n);
```

实现一个比 `dgemm2` 更积极的寄存器分块方案，要求：

- 最多使用16个局部累加变量；
- 尽量复用加载到局部变量中的 `A` 和 `B` 元素；
- 不得把结果临时写回全局矩阵后再读取；
- 必须通过框架正确性检查。

正式测试规模n能被6整除。

### 6.5 六种循环次序

实现以下六个非分块的矩阵乘法函数：

```c
void ijk(const double *A, const double *B, double *C, int n);
void jik(const double *A, const double *B, double *C, int n);
void kij(const double *A, const double *B, double *C, int n);
void ikj(const double *A, const double *B, double *C, int n);
void jki(const double *A, const double *B, double *C, int n);
void kji(const double *A, const double *B, double *C, int n);
```

函数名表示从外层到内层的循环次序。例如 `ikj` 的循环顺序必须是 `i → k → j`。

可以使用局部标量保存重复使用的 `A`、`B` 或 `C` 元素，但不能改变循环次序。

### 6.6 六种缓存分块实现

实现：

```c
void bijk(const double *A, const double *B, double *C, int n, int b);
void bjik(const double *A, const double *B, double *C, int n, int b);
void bkij(const double *A, const double *B, double *C, int n, int b);
void bikj(const double *A, const double *B, double *C, int n, int b);
void bjki(const double *A, const double *B, double *C, int n, int b);
void bkji(const double *A, const double *B, double *C, int n, int b);
```

要求：

- `b` 是缓存块大小；
- 外层三个块循环的次序必须与函数名一致；
- 块内三个循环也采用对应次序；
- 可以假设n可以被b整除；
- 如果使代码也支持不能整除的情况，可以获得奖励分数。

### 6.7 `optimal`：综合优化

综合你学到的知识和技术，实现一个最优化的矩阵乘法代码
```c
void optimal(const double *A,
             const double *B,
             double *C,
             int n,
             int b);
```

要求：

- 同时使用缓存分块和寄存器分块；
- `b` 控制缓存块大小；
- 可以在缓存块内部使用固定大小的寄存器微内核；
- 必须保持 `C = C + A * B`；
- 同一份代码需要在 `-O0`、`-O1`、`-O2`、`-O3` 下编译运行。

框架会生成：

```text
cache_part4_o0
cache_part4_o1
cache_part4_o2
cache_part4_o3
```

## 7. 调试和正确性测试

### 7.1 空白框架的预期行为

刚下载时 `src/mygemm.c` 中的函数为空。测试程序会输出 `INVALID` 并以非零状态退出，这是正常现象。

只有看到某个函数输出时间和 GFLOP/s，而不是 `INVALID`，该函数才算通过正确性检查。

### 7.2 小规模快速测试

每完成一个阶段后重新编译，并使用小矩阵测试：

```bash
cmake --build build -j

./build/reg_reuse 6 12 24 48
./build/cache_part3 48 6
./build/cache_part4_o0 48 6
./build/cache_part4_o1 48 6
./build/cache_part4_o2 48 6
./build/cache_part4_o3 48 6
```

参数含义：

```text
reg_reuse [n1 n2 n3 ...]
cache_part3 [n] [block_size]
cache_part4_o* [n] [block_size]
```

### 7.3 推荐调试顺序

1. 完成并测试 `dgemm0`；
2. 完成 `dgemm1`～`dgemm3`；
3. 完成六种非分块循环次序；
4. 先完成一个分块函数，再按相同结构完成其余五个；
5. 最后实现 `optimal`；
6. 所有小规模测试通过后再运行正式规模。

常见错误：

- 把 `C = C + A * B` 写成 `C = A * B`；
- 每次进入函数时把 `C` 清零；
- 数组下标中的行跨度不是 `n`；
- 分块循环的上界写错；
- 块大小不能整除 `n` 时访问越界；
- 寄存器分块只写回部分结果；
- 为追求速度跳过某些乘加运算。

## 8. 正式运行与数据收集

### 8.1 建立结果目录

```bash
mkdir -p results
```

以下命令使用 `tee`，结果既显示在终端，也保存到文件。`2>&1` 会把正确性错误一起写入日志。

### 8.2 寄存器复用实验

统一测试规模：

```text
64, 128, 256, 512, 1024, 2048
```

运行：

```bash
./build/reg_reuse 64 128 256 512 1024 2048 \
  2>&1 | tee results/register_reuse.txt
```

收集 `dgemm0`、`dgemm1`、`dgemm2`、`dgemm3` 在每个规模下的：

- 正确性状态；
- 执行时间（秒）；
- GFLOP/s。

### 8.3 循环次序实验

使用最大规模 `n=2048`。此处的 `b` 只供同一测试程序中的分块函数使用，先固定为 64：

```bash
./build/cache_part3 2048 64 \
  2>&1 | tee results/loop_orders.txt
```

从输出中收集六种非分块函数的时间和 GFLOP/s：

```text
ijk, jik, kij, ikj, jki, kji
```

### 8.4 缓存块大小实验

先使用 `n=1024` 筛选块大小，依次测试：

```text
b = 16, 32, 64, 128, 256
```

这些块大小都能整除 1024 和 2048。

```bash
: > results/cache_block_sweep.txt

for b in 16 32 64 128 256; do
  echo "===== block_size=${b} =====" | tee -a results/cache_block_sweep.txt
  ./build/cache_part3 1024 "$b" \
    2>&1 | tee -a results/cache_block_sweep.txt
done
```

每次运行都会同时输出非分块和分块算法。非分块算法与 `b` 无关，只需记录第一次的非分块结果；对每个 `b` 记录六种分块算法：

```text
bijk, bjik, bkij, bikj, bjki, bkji
```

根据 `n=1024` 的结果选出表现最好的两个候选块大小，然后在 `n=2048` 上确认。例如候选值为 64 和 128 时：

```bash
: > results/cache_block_2048.txt

for b in 64 128; do
  echo "===== n=2048, block_size=${b} =====" \
    | tee -a results/cache_block_2048.txt
  ./build/cache_part3 2048 "$b" \
    2>&1 | tee -a results/cache_block_2048.txt
done
```

把示例中的 64 和 128 替换为你在 1024 规模下选出的实际候选值。最终为 `optimal` 选择在 2048 规模下表现更好的块大小。

`cache_part3` 每次都会同时运行六种非分块算法和六种分块算法，因此 2048 规模可能运行较长时间。开始测试后应等待程序正常结束，不要仅因某个循环次序较慢就中断。

### 8.5 编译优化级别实验

假设缓存分块实验中选出的块大小为 `64`，在最大规模 `n=2048` 上运行：

```bash
for opt in 0 1 2 3; do
  ./build/cache_part4_o${opt} 2048 64 \
    2>&1 | tee "results/optimal_O${opt}.txt"
done
```

如果最佳块大小不是 64，请把命令中的 64 替换为实际值。四个优化级别必须使用相同的 `n=2048` 和相同的 `b`。

### 8.6 重复运行

正式结果建议重复 3 次并取中位数。示例：

```bash
: > results/optimal_O3_repeated.txt

for run in 1 2 3; do
  echo "===== run=${run} =====" | tee -a results/optimal_O3_repeated.txt
  ./build/cache_part4_o3 2048 64 \
    2>&1 | tee -a results/optimal_O3_repeated.txt
done
```

如果最佳块大小不是 64，请同样替换这里的块大小。

重复运行期间不要改变源代码、编译选项、矩阵规模或块大小。尽量关闭占用 CPU 的其他程序。

### 8.7 完整批量检查

所有函数完成后，可执行框架自带的默认批量测试：

```bash
bash scripts/run_all.sh mydata.txt
```

结果保存在：

```text
results/mydata.txt
```

默认批量测试规模较大，运行时间可能明显长于前面的快速测试。请在全部小规模测试通过后再运行。

## 9. 实验数据整理

### 9.1 必须记录运行环境

将以下命令输出保存到文件：

```bash
{
  echo "===== CPU ====="
  lscpu
  echo "===== Compiler ====="
  gcc --version
  echo "===== CMake ====="
  cmake --version
  echo "===== System ====="
  uname -a
} > results/environment.txt
```

如果使用 Clang，把 `gcc --version` 改为 `clang --version`。

### 9.2 表格 1：寄存器复用

| n | dgemm0 GFLOP/s | dgemm1 GFLOP/s | dgemm2 GFLOP/s | dgemm3 GFLOP/s |
|---:|---:|---:|---:|---:|
| 64 | | | | |
| 128 | | | | |
| 256 | | | | |
| 512 | | | | |
| 1024 | | | | |
| 2048 | | | | |

### 9.3 表格 2：六种循环次序

| 算法 | 时间/s | GFLOP/s | 正确性 |
|---|---:|---:|---|
| ijk | | | |
| jik | | | |
| kij | | | |
| ikj | | | |
| jki | | | |
| kji | | | |

### 9.4 表格 3：缓存块大小

至少为每个分块函数记录不同 `b` 下的 GFLOP/s。也可以为每个函数分别绘图。

| b | bijk | bjik | bkij | bikj | bjki | bkji |
|---:|---:|---:|---:|---:|---:|---:|
| 16 | | | | | | |
| 32 | | | | | | |
| 64 | | | | | | |
| 128 | | | | | | |
| 256 | | | | | | |

在表格下另列两个候选块在 `n=2048` 下的确认结果。

### 9.5 表格 4：综合优化

| 优化级别 | n | b | 时间/s | GFLOP/s | 正确性 |
|---|---:|---:|---:|---:|---|
| -O0 | 2048 | | | | |
| -O1 | 2048 | | | | |
| -O2 | 2048 | | | | |
| -O3 | 2048 | | | | |

### 9.6 必须绘制的图

1. `dgemm0`～`dgemm3` 的 GFLOP/s 随 `n` 变化曲线；
2. 六种循环次序的 GFLOP/s 对比图；
3. 分块算法的 GFLOP/s 随块大小变化图；
4. `optimal` 在 `-O0`～`-O3` 下的 GFLOP/s 对比图。

图中必须标明标题、横轴、纵轴、单位、矩阵规模和块大小。


## 10. Bonus 任务

Bonus 为可选任务，并支持使用AI工具完成。

### 10.1 Bonus 1：实现 Strassen 并挑战 `optimal -O3`

实现 Strassen 矩阵乘法，测试它能否超过你在必做部分完成的、使用 `-O3` 编译的 `optimal`。

#### 10.1.1 文件和接口

在仓库中新建 `bonus/`，至少包含：

```text
bonus/strassen.c
bonus/strassen.h
bonus/strassen_bench.c
```

推荐接口：

```c
void strassen(const double *A,
              const double *B,
              double *C,
              int n,
              int cutoff,
              double *workspace);
```

要求：

- 仍然计算 `C = C + A * B`；
- `cutoff` 是停止递归、切换到普通或分块矩阵乘法的阈值；
- `workspace` 在计时开始前分配，不能在每层递归中反复 `malloc`/`free`；
- 至少支持 `n = 256, 512, 1024, 2048`；
- 必须使用框架的参考 BLAS 验证结果；
- 不允许调用外部 BLAS 完成 Strassen 的叶子计算；
- 可以复用你在 `src/mygemm.c` 中编写的分块或寄存器微内核。

`bonus/strassen_bench.c` 应接受：

```text
bonus_strassen [n] [cutoff]
```

并输出正确性、执行时间和 GFLOP/s。

#### 10.1.2 编译

完成上述文件后，可使用以下命令编译：

```bash
gcc -O3 -std=c11 -D_POSIX_C_SOURCE=200809L \
  -Wall -Wextra -Wpedantic \
  -Iinclude -Ithird_party/labblas/include \
  bonus/strassen.c bonus/strassen_bench.c \
  src/util.c third_party/labblas/src/cblas.c \
  -lm -o build/bonus_strassen
```

不得为 Strassen 单独增加 `-ffast-math`、`-march=native` 或其他 `optimal -O3` 没有使用的额外优化选项。

#### 10.1.3 选择递归阈值

先在 `n=1024` 上测试：

```bash
: > results/strassen_cutoff.txt

for cutoff in 32 64 128 256; do
  ./build/bonus_strassen 1024 "$cutoff" \
    2>&1 | tee -a results/strassen_cutoff.txt
done
```

选出正确且最快的 `cutoff`，再测试最大规模。假设最佳值为 128：

```bash
for n in 256 512 1024 2048; do
  ./build/bonus_strassen "$n" 128 \
    2>&1 | tee "results/strassen_${n}.txt"
done
```

#### 10.1.4 公平对比

使用相同的 `n=2048`，分别将 Strassen 和 `optimal -O3` 运行 3 次并取中位数：

```bash
: > results/bonus_strassen_vs_optimal.txt

for run in 1 2 3; do
  echo "===== optimal run=${run} =====" \
    | tee -a results/bonus_strassen_vs_optimal.txt
  ./build/cache_part4_o3 2048 64 \
    2>&1 | tee -a results/bonus_strassen_vs_optimal.txt

  echo "===== strassen run=${run} =====" \
    | tee -a results/bonus_strassen_vs_optimal.txt
  ./build/bonus_strassen 2048 128 \
    2>&1 | tee -a results/bonus_strassen_vs_optimal.txt
done
```

把示例中的块大小 64 和阈值 128 替换为你的最佳参数。

公平性要求：

- 两个程序均为单线程；
- 使用同一编译器和 `-O3`；
- 使用相同矩阵规模和相同重复次数；
- 输入初始化、正确性检查和计时边界一致；
- 矩阵及 Strassen 工作区的分配不计入核心乘法时间；
- 两种实现都必须通过正确性检查后才能比较性能。

提交表格：

| 实现 | n | 参数 | 中位时间/s | GFLOP/s | 正确性 |
|---|---:|---|---:|---:|---|
| optimal -O3 | 2048 | b= | | | |
| Strassen | 2048 | cutoff= | | | |

明确回答：你的 Strassen 是否超过了 `optimal -O3`？没有超过也可以获得 Bonus，但必须如实报告结果并指出主要性能开销。

### 10.2 Bonus 2：使用 AI agent 优化开源 BLAS

本任务使用 [BLIS 官方仓库](https://github.com/flame/blis) 的固定版本 **2.1**。BLIS 是采用 [BSD-3-Clause 许可证](https://github.com/flame/blis/blob/2.1/LICENSE) 的开源 BLAS-like 框架，其 GEMM 实现将分块、数据打包和微内核分离，适合作为性能优化实验对象。官方的[构建说明](https://github.com/flame/blis/blob/2.1/docs/BuildSystem.md)、[测试说明](https://github.com/flame/blis/blob/2.1/docs/Testsuite.md)和[插件说明](https://github.com/flame/blis/blob/2.1/docs/PluginHowTo.md)可作为参考。


#### 10.2.1 获取固定版本

```bash
mkdir -p bonus
cd bonus

git clone --branch 2.1 --depth 1 https://github.com/flame/blis.git
cd blis
git switch -c ai-gemm-opt

git rev-parse HEAD | tee ../../results/blis_commit.txt
```


#### 10.2.2 建立单线程基线

首先使用可移植的 `generic` 配置建立基线：

```bash
./configure generic
make -j
make checkblis-fast
```

限制为单线程：

```bash
export BLIS_NUM_THREADS=1
export OMP_NUM_THREADS=1
```

按照 BLIS `testsuite/input.general` 文件中的注释，将测试设置为：

- 重复 3 次；
- 只测试双精度实数 `d`；
- 矩阵存储方式固定为一种，例如行优先 `r`，且“测试全部存储组合”设为 `0`；
- 问题规模的起始值、最大值和步长分别设为 `256`、`2048`、`256`；
- 启用完整错误检查；
- 单线程运行。

在 `testsuite/input.operations` 中关闭其他操作，只启用 GEMM；把三个维度参数都设为 `-1`，使 `m=n=k`，并固定测试非转置的 `nn` 情形。保存原文件副本和实际测试配置：

```bash
cd testsuite
cp input.general input.general.original
cp input.operations input.operations.original
# 按上述要求编辑 input.general 和 input.operations 后保存一份记录
cp input.general ../../../results/blis_input.general
cp input.operations ../../../results/blis_input.operations
make -j

./test_libblis.x \
  2>&1 | tee ../../../results/blis_baseline.txt

cd ..
```

基线和优化后结果都必须包含 `n=256, 512, 1024, 1536, 2048`，并且正确性结果全部为 `PASS`。BLIS 测试程序会在 3 次重复中报告最佳性能；基线和优化版必须采用相同规则。

#### 10.2.3 让 AI agent 进行优化

可以使用任意能够读写代码、执行构建和运行测试的 AI coding agent。建议把任务限制在双精度 GEMM、单线程和当前 CPU，优先让 agent 检查：

- GEMM 微内核；
- `MR`、`NR`、`MC`、`KC`、`NC` 等块大小；
- 数据打包和内存访问；
- 编译器向量化；
- BLIS 2.1 的插件或自定义内核机制；
- 当前 CPU 支持的安全指令集。

可以使用以下提示词作为起点：

```text
请针对这台机器优化 BLIS 2.1 generic 配置下的单线程双精度 GEMM。
先读取 lscpu、建立并保存基线，再分析热点和相关微内核/块大小。
每次只做一个可解释的小修改；修改后必须重新编译，运行
make checkblis-fast，并使用完全相同的矩阵规模、输入配置和线程数重测。
不得调用其他 BLAS，不得减少计算、降低精度、修改测试规模或关闭正确性检查。
如果修改没有稳定提升，请回退该修改。最后给出修改文件、原因、测试命令、
正确性结果、各规模性能以及相对基线的加速比。
```

必须监督 agent 的操作。不得让 agent 修改基准规模、重复次数、计时方式或正确性判据来制造性能提升。

#### 10.2.4 每轮优化流程

每轮修改后执行：

```bash
make -j
make checkblis-fast

cd testsuite
./test_libblis.x \
  2>&1 | tee "../../../results/blis_agent_round_N.txt"
cd ..
```

把 `N` 替换为轮次编号。只有同时满足以下条件的修改才能保留：

1. 编译成功；
2. `make checkblis-fast` 通过；
3. GEMM 测试全部 `PASS`；
4. 相同规模下的性能提升能够重复出现。

最后保存修改：

```bash
git status --short > ../../results/blis_git_status.txt
git diff --stat > ../../results/blis_diff_stat.txt
git diff > ../../results/blis_ai_optimization.patch
```

不要直接向 BLIS 上游提交 PR，除非教师另行要求。

#### 10.2.5 汇报要求

提交修改后的代码库和一份 `bonus/blis_ai_report.md`，至少包含你所进行的代码优化的介绍与说明，相关实验结果和分析，以及对AI Agent优化底层代码能力的讨论和评价。



参考性能表格：

| n | BLIS generic 基线 GFLOP/s | AI 优化后 GFLOP/s | 加速比 | 正确性 |
|---:|---:|---:|---:|---|
| 256 | | | | |
| 512 | | | | |
| 1024 | | | | |
| 1536 | | | | |
| 2048 | | | | |

加速比按以下方式计算：

```text
speedup = 优化后 GFLOP/s / 基线 GFLOP/s
```

AI agent 未能获得性能提升并不等于任务失败。只要过程可复现、正确性未被破坏、尝试和负面结果被完整记录，仍可获得相应 Bonus。伪造数据、隐藏失败测试或通过修改基准制造提升将被触发。
