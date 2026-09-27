# 交我算 π 2.0：Lab 操作讲义

适用：课程学生首次在 π 2.0 运行 DataLab 和 MatrixLab。命令需在标注的位置执行。

- 本地目录示例为 /path/to/course-labs，请替换为课程学生框架的实际解压目录。
- 将 your_username 替换为本人超算账号。HPC_USER 只是在本地终端使用的变量，新终端需重新设置。
- 课程根目录应包含 labs/lab1-datalab 和 labs/lab2-matrix。
- 不上传本地 build、CMakeCache.txt 或可执行文件，集群上重新编译。
- 教学账号按平台要求在校内网络登录。

## 1. 本地上传和登录

```bash
HPC_USER=your_username
cd /path/to/course-labs
rsync -av --exclude=".git/" --exclude="build*/" \
  --exclude="results/" --exclude="btest" \
  --exclude="btest.exe" ./ \
  "${HPC_USER}@data.hpc.sjtu.edu.cn:course-labs/"
ssh "${HPC_USER}@pilogin.hpc.sjtu.edu.cn"
```

已有别名时可用 `ssh jiaowosuan`。此别名仅在配置过的电脑上生效。
重复上传会更新同名文件。若在远端改过源码，先保存远端修改。

## 2. 在登录节点申请交互式计算资源

```bash
cd ~/course-labs
srun -p cpu -N 1 -n 1 -c 1 -t 00:20:00 \
  --cpu-bind=cores --pty /bin/bash
hostname
gcc --version
cmake --version
```

只有资源分配完成后才执行编译和实验。当前框架要求 CMake ≥ 3.16。
若工具不存在或版本过旧，在计算节点运行 `module avail cmake`，然后用
`module load` 加载列表中实际存在的模块名，再确认版本。
不要直接在登录节点运行实验。交互调试结束输入 `exit` 释放资源。

## 3. 在计算节点运行 DataLab

```bash
cd ~/course-labs/labs/lab1-datalab
make -j1
make selftest
./btest -f bitCount
make grade
./dlc bits.c
```

先按题面填写 bits.c 的姓名学号，并完成自己的实现。
`selftest` 检查测试框架，`grade` 检查学生答案。
`dlc` 只检查它支持的编码规则，新增 FP8 题需结合题面自行检查。

## 4. 在计算节点运行 MatrixLab

```bash
cd ~/course-labs/labs/lab2-matrix
cmake -S . -B build-pi2 -DBUILD_TESTING=ON \
  -DLAB_USE_SYSTEM_BLAS=OFF
cmake --build build-pi2 --parallel 1
(cd build-pi2 && ctest --output-on-failure)
./build-pi2/reg_reuse 6 12 24 48
./build-pi2/cache_part3 48 6
for opt in 0 1 2 3; do
  ./build-pi2/cache_part4_o${opt} 48 6
done
```

无需安装 MKL 或 OpenBLAS。ctest 通过仅说明框架和参考 BLAS 自检通过。
学生函数还需要通过后续各个可执行文件的数值检查。出现 INVALID 时先修正代码。
当前框架已为各测试目标设置相应优化级别，不额外添加全局优化选项。

## 5. DataLab 批处理脚本

将以下内容保存为远端 course-labs/lab1.slurm：

```bash
#!/bin/bash
#SBATCH -J datalab
#SBATCH -p cpu -N 1 -n 1 -c 1
#SBATCH -t 00:20:00
#SBATCH -o results/lab1-%j.log

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
make -C labs/lab1-datalab -j1
make -C labs/lab1-datalab selftest
make -C labs/lab1-datalab grade
```

## 6. MatrixLab 批处理脚本

将以下内容保存为远端 course-labs/lab2.slurm：

```bash
#!/bin/bash
#SBATCH -J matrixlab
#SBATCH -p cpu -N 1 -n 1 -c 1
#SBATCH -t 00:20:00
#SBATCH -o results/lab2-%j.log

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
B="build/pi2-lab2-$SLURM_JOB_ID"
cmake -S labs/lab2-matrix -B "$B" \
  -DBUILD_TESTING=ON \
  -DLAB_USE_SYSTEM_BLAS=OFF
cmake --build "$B" --parallel 1
(cd "$B" && ctest --output-on-failure)
srun --cpu-bind=cores "$B/reg_reuse" 6 12 24 48
srun --cpu-bind=cores "$B/cache_part3" 48 6
for opt in 0 1 2 3; do
  srun --cpu-bind=cores "$B/cache_part4_o$opt" 48 6
done
```

每个作业生成独立的构建目录。此脚本先完成小规模验证。
set -euo pipefail 保证失败能反映在作业退出码中。
若节点需要软件模块，在 cmake 命令之前加入经过确认的 module load 命令。

## 7. 提交与查看状态（登录节点）

```bash
cd ~/course-labs
mkdir -p results
sbatch lab1.slurm
sbatch lab2.slurm
squeue -u "$USER"
```

必须在提交前创建 results/，因为 Slurm 会在脚本开始前打开日志文件。
把以下 123456 换成提交后获得的作业编号：

```bash
tail -f results/lab2-123456.log
sacct -j 123456 --format=JobID,State,ExitCode,Elapsed
```

tail -f 用 Ctrl-C 结束查看。PD 表示排队，R 表示运行。
确认最终状态、退出码和程序正确性输出。作业从 squeue 消失不代表成功。
只有确实需要取消时才执行 `scancel 123456`。

## 8. 正式性能实验

先完成小规模检查，再按课程统一题面设置 n、b 和重复次数，并相应增加 #SBATCH -t 时限。
当前源码的默认参数如下，它们用于解释当前程序行为，不能替代课程正式要求：

| 程序 | 当前驱动默认参数 |
| --- | --- |
| reg_reuse | 66、126、258、510、1026、2046 |
| cache_part3 | n=2000，b=10 |
| cache_part4_o0..o3 | n=2040，b=60 |

当前 dgemm3 的约定涉及 6 的倍数。若题面规模与整除条件冲突，先由教师统一，学生不要自行改变实验口径。
以下仅演示怎样在 lab2.slurm 末尾追加重复运行，不定义正式规模：

```bash
for rep in 1 2 3; do
  srun --cpu-bind=cores "$B/cache_part4_o3" 2040 60
done
```

对 O0、O1、O2、O3 的比较须保持同样的 n 和 b。
建议每种配置串行运行 3 次，记录中位数，并记录 hostname、lscpu、gcc --version、编译选项及正确性状态。
绑核降低迁移带来的干扰，共享节点仍可能有缓存、带宽和频率干扰。
当前 scripts/submit.sh 只是本地运行包装，不调用 sbatch，不要在登录节点直接执行它。

## 9. 下载结果（本地电脑）

```bash
HPC_USER=your_username
mkdir -p pi2-results
rsync -av \
  "${HPC_USER}@data.hpc.sjtu.edu.cn:course-labs/results/" \
  ./pi2-results/
```

按实验批次保存日志。关闭 SSH 通常不会停止已提交的 sbatch 作业。

## 官方参考

- [SSH 登录](https://docs.hpc.sjtu.edu.cn/login/sshlogin.html)
- [文件传输](https://docs.hpc.sjtu.edu.cn/transport/transportsolution.html)
- [Slurm 作业管理](https://docs.hpc.sjtu.edu.cn/job/slurm.html)
- [队列说明](https://docs.hpc.sjtu.edu.cn/job/partition.html)
- [π 2.0 CPU 环境](https://docs.hpc.sjtu.edu.cn/job/kos.html)

命令依据 2026-09-27 的课程仓库和平台文档整理。队列权限与软件环境以实际账号查询为准。
