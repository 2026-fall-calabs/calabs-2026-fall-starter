# 交我算 π 2.0：Lab 操作讲义

适用：课程学生首次在 π 2.0 运行 DataLab 和 MatrixLab。命令需在标注的位置执行。

- 从教师发布的交大云盘链接下载学生框架；将解压后的课程根目录命名为 course-labs。
- 将 your_username 替换为本人超算账号。命令中的 jiaowosuan 和 jiaowosuan-data 是下面配置的本地别名。
- 课程根目录应包含 labs/lab1-datalab 和 labs/lab2-matrix。
- 不上传本地 build、CMakeCache.txt 或可执行文件，集群上重新编译。
- 教学账号按平台要求在校内网络登录。

## 1. 云盘获取、SSH 配置与 VS Code 连接

### 1.1 从云盘获取学生框架

在本地浏览器打开教师提供的交大云盘链接，完成页面要求的登录或提取码验证后下载。
解压并确认 `course-labs/labs/` 下有两个 Lab 目录。保留教师发布的版本号。
课程采用云盘交作业，代码检查在计算节点运行，不依赖 GitHub workflow。

云盘分享页面不一定是文件直链，不能直接把分享页面地址当成 `wget` 下载地址。
本教程采用“浏览器下载到本地，再传到数据节点”的流程。

### 1.2 在自己的电脑编辑 SSH config

先在本地终端运行 `ssh -V`，确认已有 OpenSSH 客户端。

| 系统 | 配置文件位置 | 编辑方式 |
| --- | --- | --- |
| macOS / Linux | `~/.ssh/config` | 终端中用 nano，或用 VS Code 打开该文件 |
| Windows | `C:\Users\你的用户名\.ssh\config` | PowerShell 中用记事本，或用 VS Code 打开 |

macOS / Linux：

```bash
mkdir -p ~/.ssh
touch ~/.ssh/config
nano ~/.ssh/config
```

把下一节配置追加到文件中；保留已有主机条目。nano 中用 Ctrl-O、Enter 保存，Ctrl-X 退出。
文件权限可在本地设置为：

```bash
chmod 700 ~/.ssh
chmod 600 ~/.ssh/config
```

Windows PowerShell：

```powershell
New-Item -ItemType Directory -Force "$env:USERPROFILE\.ssh"
notepad "$env:USERPROFILE\.ssh\config"
```

文件名必须是 `config`，没有 `.txt` 后缀。上述路径是本地用户目录，不是课程仓库
里的 `.vscode`，也不是远端服务器上的 `~/.ssh/config`。

### 1.3 添加登录与传输别名

把下列内容写入本地 `config`，将两处 `your_username` 都换成本人超算用户名。
如果已有同名 `Host` 条目，编辑原条目，避免重复定义。

```sshconfig
Host jiaowosuan
    HostName pilogin.hpc.sjtu.edu.cn
    User your_username
    Port 22
    ServerAliveInterval 60
    ServerAliveCountMax 3

Host jiaowosuan-data
    HostName data.hpc.sjtu.edu.cn
    User your_username
    Port 22
    ServerAliveInterval 60
    ServerAliveCountMax 3
```

`Host` 是自己取的简称，`HostName` 是平台入口，`User` 是超算账号。
登录别名用于编辑文件和提交作业；数据别名用于文件上传下载。

在本地终端测试：

```bash
ssh jiaowosuan
hostname
pwd
exit
```

第一次连接先核对主机身份，再按提示确认。按平台要求输入密码或完成认证。
别名只简化命令，不会自动免除密码认证；不要在 config 里填写密码。

### 1.4 可选：平台免密证书

需要免密登录时，通过[超算账号管理平台](https://my.hpc.sjtu.edu.cn/)按官方流程
申请证书。配置匹配的私钥和有效证书，例如在相应的 `Host` 段内增加：

```sshconfig
    IdentityFile ~/.ssh/id_ed25519
    CertificateFile ~/.ssh/id_ed25519-cert.pub
```

文件名以自己实际持有的文件为准；Windows 可使用 `C:/Users/你的用户名/.ssh/...`
形式的路径。证书过期后需按平台流程续签。平台已改用证书认证，单独使用
`ssh-copy-id` 或添加 `authorized_keys` 不能完成该免密配置。
私钥和证书留在本地，不放进课程代码或云盘作业包。

### 1.5 首次上传源码（本地电脑）

先进入 `course-labs` 的上一级目录，再执行；Windows PowerShell 也可使用：

```bash
scp -r ./course-labs jiaowosuan-data:
ssh jiaowosuan
cd ~/course-labs
ls labs
```

上传前检查目录仅含学生框架和自己的源码。后续更新先保存远端修改，再合并新版本，
避免用本地旧代码覆盖 VS Code 远程编辑的内容。

macOS / Linux 也可用 rsync 过滤生成文件：

```bash
cd /path/to/course-labs
rsync -av --exclude=".git/" --exclude="build*/" \
  --exclude="results/" --exclude="btest" \
  --exclude="btest.exe" ./ \
  jiaowosuan-data:course-labs/
```

### 1.6 VS Code Remote-SSH

1. 在本地 VS Code 扩展页面安装 Microsoft 发布的 **Remote - SSH**。
2. 先在独立的本地终端运行 `ssh jiaowosuan`，保持该普通 SSH 会话打开。平台用它
   判断用户是否活跃，只有 VS Code 后台进程时可能清理连接。
3. 在 VS Code 按 F1，执行 **Remote-SSH: Connect to Host...**，选择 `jiaowosuan`。
4. 若提示远端系统，选择 **Linux**。按提示完成密码、证书或额外认证。
5. 确认左下角显示 `SSH: jiaowosuan`。选择“打开文件夹”，输入远端的 `~/course-labs`；
   若该输入框未展开 `~`，使用普通 SSH 会话里 `pwd` 显示的主目录绝对路径。
6. 在远程窗口打开 `labs/lab1-datalab/bits.c` 或 `labs/lab2-matrix/src/mygemm.c`。
   保存会修改服务器上的文件。
7. 打开 VS Code 集成终端，按第 2 节先运行 `srun` 申请计算资源，再编译和测试。

新开的集成终端通常仍在登录节点。每次运行实验前用 `hostname` 确认位置；只有发起
`srun` 并获分配的终端进入了计算节点。编辑器连接不会自动随之迁移。
登录节点上只编辑文件、查看状态和提交作业，不使用自动编译或直接运行实验按钮。

### 1.7 VS Code 连接排错

- **密码提示未显示**：查看“输出”中的 Remote - SSH 日志，或启用本地 VS Code
  用户设置 `remote.SSH.showLoginTerminal`。
- **远端下载 VS Code Server 失败**：在本地用户设置中将
  `remote.SSH.localServerDownload` 设为 `always`，让本地下载后传到远端；本地仍需
  能访问微软下载服务。扩展及其依赖的下载还可能需要另外处理。
- **别名找不到**：检查编辑的是当前本地用户的 config，并确认文件没有 `.txt` 后缀。
- **证书失效或账号认证失败**：先在普通终端检查 `ssh jiaowosuan`，再检查证书有效期、
  校内网络和平台认证提示。
- **终端 SSH 成功但 VS Code 反复断开**：保持普通 SSH 会话，查看 Remote - SSH 日志；
  平台入口可能分配不同登录节点，必要时按平台和 VS Code 官方文档排查连接复用。

在“首选项：打开用户设置(JSON)”中合并以下字段，保留已有其他设置：

```json
{
  "remote.SSH.showLoginTerminal": true,
  "remote.SSH.localServerDownload": "always"
}
```

这是本地 VS Code 用户设置，不需要创建仓库 `.vscode/settings.json`。

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
mkdir -p pi2-results
rsync -av \
  jiaowosuan-data:course-labs/results/ \
  ./pi2-results/
```

按实验批次保存日志。关闭 SSH 通常不会停止已提交的 sbatch 作业。

Windows 或未安装 rsync 的电脑可在本地新建的收集目录中执行
`scp -r jiaowosuan-data:course-labs/results ./`，下载整个 results 子目录。

## 10. 通过交大云盘提交作业

先在远程编辑器保存最终代码并完成计算节点上的检查，再回到**本地终端**下载要提交
的源码。下面示例同时收集两个 Lab；正式提交时按各 Lab 题面分别整理。

```bash
mkdir pi2-submission
scp jiaowosuan-data:course-labs/labs/lab1-datalab/bits.c ./pi2-submission/
scp jiaowosuan-data:course-labs/labs/lab2-matrix/src/mygemm.c ./pi2-submission/
```

若目录已存在，先检查其中版本，或使用带日期的新目录名。随后：

1. 加入题面要求的报告和实验日志，检查文件内容确为最后一次修改。
2. 通过系统压缩功能分别生成各 Lab 的作业包。建议命名“学号_姓名_Lab编号.zip”，
   以课程实际命名要求为准。
3. 上传到教师指定的交大云盘收件入口，填写要求的身份信息。
4. 重新下载已上传的包，核对源码与报告，并保留上传时间及版本记录。

不用上传 build、缓存、可执行文件或 SSH 配置。无需创建 GitHub PR，也没有自动
workflow 反馈；公开测试自行运行，教师下载原始提交后统一评分。云盘上传本身不会
运行测试。提交入口、所需文件与截止时间以教师通知为准。

## 官方参考

- [SSH 登录](https://docs.hpc.sjtu.edu.cn/login/sshlogin.html)
- [免密证书与账号管理](https://docs.hpc.sjtu.edu.cn/accounts/security.html)
- [平台 VS Code 使用说明](https://docs.hpc.sjtu.edu.cn/login/vscode.html)
- [VS Code Remote-SSH](https://code.visualstudio.com/docs/remote/ssh)
- [VS Code 远程连接排错](https://code.visualstudio.com/docs/remote/troubleshooting)
- [文件传输](https://docs.hpc.sjtu.edu.cn/transport/transportsolution.html)
- [Slurm 作业管理](https://docs.hpc.sjtu.edu.cn/job/slurm.html)
- [队列说明](https://docs.hpc.sjtu.edu.cn/job/partition.html)
- [π 2.0 CPU 环境](https://docs.hpc.sjtu.edu.cn/job/kos.html)

命令依据 2026-09-27 的课程仓库和平台文档整理。队列权限与软件环境以实际账号查询为准。
