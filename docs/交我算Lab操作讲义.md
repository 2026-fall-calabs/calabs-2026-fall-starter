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

### 1.4 可选：学生免密登录与证书申请

本节适用于**账号已开放免密证书申请，且普通密码登录正常**的同学。课程教学账号
是否开放申请，以实际管理页面和管理员确认为准。没有申请入口或申请失败时，继续
使用密码登录并联系助教。免密证书不会解除教学账号的校内网络登录限制。

#### 第一步：登录平台并申请证书

1. 在自己的电脑上打开[超算账号管理平台](https://my.hpc.sjtu.edu.cn/)。
2. 登录后核对当前超算账号，确保是老师发给你的教学账号。按页面提示在“个人主页”
   补充所需的 jAccount / 邮箱绑定，已有绑定不要随意更改。
3. 找到免密证书申请功能。没有自己的密钥时可使用“一键生成”；已有密钥时，上传
   对应公钥文件（例如 `id_ed25519.pub`），或粘贴公钥文本。不要上传私钥。
4. 按页面提示完成授权。一键生成后，将下载目录里的**私钥与证书两个文件**移入
   本地 `.ssh`，操作见下一步；提交已有公钥时，继续使用原来的私钥。

#### 第二步：将 Downloads 中的两个文件移入本地 .ssh

下面以一键生成后下载的 `id_ed25519` 与 `id_ed25519-cert.pub` 为例：

| 下载的文件 | 用途 |
| --- | --- |
| `id_ed25519` | 私钥，配置中的 `IdentityFile` 指向它 |
| `id_ed25519-cert.pub` | 证书，配置中的 `CertificateFile` 指向它 |

本步骤只需要这两个文件。文件名以实际下载结果为准；如果是 `id_rsa` 和
`id_rsa-cert.pub`，后面的命令与配置一并替换。如果浏览器使用其他下载目录，也要
相应修改 `Downloads` 路径。

**在自己的电脑上移动文件。目标 `.ssh` 若已有同名文件，先停止并核对用途，保留
原文件；可另建目录存放新文件，再相应修改 config。**

macOS / Linux 本地终端：

```bash
mkdir -p ~/.ssh
mv -i ~/Downloads/id_ed25519 ~/.ssh/
mv -i ~/Downloads/id_ed25519-cert.pub ~/.ssh/
chmod 700 ~/.ssh
chmod 600 ~/.ssh/id_ed25519
```

`mv -i` 遇到同名目标文件时会询问是否覆盖，应先输入 `n`，核对原文件用途。
移动完成后，私钥和证书分别位于 `~/.ssh/id_ed25519` 与
`~/.ssh/id_ed25519-cert.pub`。

Windows 本地 PowerShell：

```powershell
Set-Location "$env:USERPROFILE"
New-Item -ItemType Directory -Force .ssh
Move-Item ./Downloads/id_ed25519 ./.ssh/
Move-Item ./Downloads/id_ed25519-cert.pub ./.ssh/
```

先切换到自己的用户目录，随后将两个文件移到该目录下的 `.ssh`。Windows 不执行
上面的 `chmod` 命令。也可在文件资源管理器中剪切这两个文件，粘贴到
`C:/Users/你的用户名/.ssh/`。移动报同名冲突时先核对，不覆盖已有文件。
私钥、个人配置和证书不放进课程代码或云盘作业包。

#### 第三步：修改自己的 SSH config

两个文件移入 `.ssh` 后，在第 1.3 节已有的 `Host jiaowosuan` 和
`Host jiaowosuan-data` 两个配置段内，**分别追加**下面三行，保留原有
`HostName`、`User` 等字段：

```sshconfig
    IdentityFile ~/.ssh/id_ed25519
    CertificateFile ~/.ssh/id_ed25519-cert.pub
    IdentitiesOnly yes
```

`IdentityFile` 指向移动后的私钥，`CertificateFile` 指向匹配证书。使用已有密钥
申请时，`IdentityFile` 改为原私钥的实际路径。证书对应的账号应与 `User` 一致。
Windows 的 config 同样可使用 `~/.ssh/...`，也可写成
`C:/Users/你的用户名/.ssh/...`；路径包含空格时用双引号括起。

`IdentitiesOnly yes` 避免额外尝试 `ssh-agent` 提供的其他密钥，适合同时配置了多个
服务器账号的电脑。它不会禁用密码或键盘交互认证，也不免除私钥自身的 passphrase。
平台已改用证书认证，单独使用 `ssh-copy-id` 或向 `authorized_keys` 添加公钥
不能替代证书申请。

#### 第四步：直接登录，按需验证免密

配置完成后，日常在本地终端直接运行：

```bash
ssh jiaowosuan
```

VS Code Remote-SSH 同样选择 `jiaowosuan`。如果普通登录仍要求输入超算账号密码，
应检查文件路径、账号和证书有效期。如果提示 `passphrase`，这是私钥的本地保护
口令，仍可能需要输入。

**可选验证**：可临时仅允许公钥类认证（包括 SSH 证书），避免回退到账号密码：

```bash
ssh -o PreferredAuthentications=publickey jiaowosuan
```

`-o` 为本次 SSH 连接传入临时选项。这里的选项避免证书认证失败后回退到账号密码，
便于判断免密是否成功；它只对本次命令生效，**日常登录不用加 `-o`，也不需要将
`PreferredAuthentications` 写入 config**。

成功进入远端后，可运行 `hostname` 确认，再用 `exit` 退出。数据节点也可将别名
替换为 `jiaowosuan-data` 单独测试。

如需检查证书详情，在本地终端进入 `.ssh` 目录。macOS / Linux：

```bash
cd ~/.ssh
```

Windows PowerShell：

```powershell
Set-Location "$env:USERPROFILE/.ssh"
```

然后查看证书的 `Valid` 有效期和 `Principals` 账号信息：

```bash
ssh-keygen -L -f id_ed25519-cert.pub
```

#### 第五步：续签与排错

- **找不到文件**：核对本地目录、文件名、扩展名，以及 config 中的实际路径。
- **Permission denied**：检查私钥与证书是否匹配、`User` 是否正确、证书是否有效。
  可以先用普通 `ssh jiaowosuan` 按提示进行密码认证。
- **证书过期**：回到管理平台重新申请。使用原公钥时更新证书文件；重新生成密钥时
  同时更新私钥与证书，再重复验证。
- **账号未开放申请**：使用密码登录并请助教向平台确认权限。

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
srun -p cpu -N 1 -n 1 -c 1 -t 03:00:00 \
  --cpu-bind=cores --pty /bin/bash -l
hostname
gcc --version
module avail cmake
module load cmake
cmake --version
```

本教程统一申请最长 3 小时运行时间（`03:00:00`），不含排队时间。
编译、测试和交互终端空闲均计入时限；提前结束就会提前释放资源。

只有资源分配完成后才执行编译和实验。`/bin/bash -l` 启动登录 shell，以初始化
计算节点的软件模块环境。`module load cmake` 加载默认版本，本教程命令建议 CMake/CTest ≥ 3.20。
若默认模块不可用或版本过旧，从 `module avail cmake` 的结果中选择实际存在的版本，
再用 `module load 完整模块名` 加载并确认版本。3.16～3.19 的 CTest 兼容写法见第 4 节。官方 KOS 页面列出的示例是
`module load cmake/3.26.3-gcc-8.5.0`，以当前节点的实际查询结果为准。
不要直接在登录节点运行实验。交互调试结束输入 `exit` 释放资源。

## 3. 在计算节点运行 DataLab

按 [Lab 1 README 第 3、10、11 节](../labs/lab1-datalab/README.md)，先在 `bits.c`
填写姓名和学号，再编译、做单题测试、运行全部测试和查看评分表：

```bash
cd ~/course-labs/labs/lab1-datalab
make
./btest -f bitCount
./btest
./btest -g
chmod u+x ./dlc
./dlc bits.c
```

每次修改 `bits.c` 后重新运行 `make`。`./btest -g` 与 `make grade` 都用于查看学生
答案的评分表，当前正确性满分为 50 分。`make selftest` 仅是框架自检，不替代学生答案测试。
`dlc` 是 x86-64 Linux 程序；它不支持新增 FP8 题的全部规则，FP8 的编码限制需要另行核对。
提交前按 README 第 13 节执行 `make clean && make`，再用 `./btest -g` 确认得分。

## 4. 在计算节点编译并检查 MatrixLab

与 [Lab 2 README 第 5 节](../labs/lab2-matrix/README.md) 使用同一套 `build/` 目录。
在新下载的源码目录中配置，保留框架默认的内置 BLAS 和 CTest 设置：

```bash
cd ~/course-labs/labs/lab2-matrix
cmake -S . -B build
cmake --build build -j1
ctest --test-dir build --output-on-failure
```

交我算申请一个 CPU 核心，编译统一使用 `-j1`，与 README 一致。不要上传本地 `build/` 或 `CMakeCache.txt`。
`ctest --test-dir` 需要 CMake/CTest ≥ 3.20。若模块只有 3.16～3.19，最后一行改为：

```bash
(cd build && ctest --output-on-failure)
```

框架本身的最低版本仍为 CMake 3.16。两个 CTest 测试只检查框架和参考 BLAS，
学生矩阵函数需按 README 第 7.2 节执行：

```bash
cmake --build build -j1
./build/reg_reuse 6 12 24 48
./build/cache_part3 48 6
./build/cache_part4_o0 48 6
./build/cache_part4_o1 48 6
./build/cache_part4_o2 48 6
./build/cache_part4_o3 48 6
```

上述命令在 `~/course-labs/labs/lab2-matrix` 中执行。空模板出现 `INVALID`、返回非零
是预期行为。完成相应函数并通过小规模检查后，再进行正式测量。
编译目标已固定：`reg_reuse`、`cache_part3` 为 `-O0`，`cache_part4_o0`～`o3` 对应四个优化级别。
不要另加全局优化选项。更新源码后运行 `cmake --build build -j1`。

## 5. DataLab 批处理脚本

课程根目录已提供 [lab1.slurm](../lab1.slurm)，无需手动创建。先填写个人信息并完成代码。
默认执行全部评分；传入函数名可以单题测试。编码规则检查按第 3 节另行执行。

```bash
#!/bin/bash -l
#SBATCH -J datalab
#SBATCH -p cpu -N 1 -n 1 -c 1
#SBATCH -t 03:00:00
#SBATCH -o results/lab1-%j.log

set -euo pipefail
: "${SLURM_JOB_ID:?请使用 sbatch 提交作业，不要在登录节点直接运行}"

if (( $# > 1 )); then
  echo "用法：sbatch lab1.slurm [函数名]" >&2
  exit 2
fi
cd "${SLURM_SUBMIT_DIR:?请从课程根目录提交}/labs/lab1-datalab"
make -j1
if (( $# == 1 )); then
  srun --cpu-bind=cores ./btest -f "$1"
else
  srun --cpu-bind=cores ./btest -g
fi
```

## 6. MatrixLab 批处理脚本

课程根目录已提供 [lab2.slurm](../lab2.slurm)，用于 README 第 8.3 节的正式循环次序实验。
其他阶段各有独立脚本，完整列表见 [Slurm 脚本与提交命令](../slurm/README.md)。
先完成相应函数并通过小规模检查，再提交正式实验：

```bash
#!/bin/bash -l
#SBATCH -J matrixlab
#SBATCH -p cpu -N 1 -n 1 -c 1
#SBATCH -t 03:00:00
#SBATCH -o results/lab2-%j.log

set -euo pipefail
: "${SLURM_JOB_ID:?请使用 sbatch 提交作业，不要在登录节点直接运行}"

cd "${SLURM_SUBMIT_DIR:?请从课程根目录提交}/labs/lab2-matrix"
module load "${CMAKE_MODULE:-cmake}"
cmake -S . -B build
cmake --build build -j1
(cd build && ctest --output-on-failure)
mkdir -p results

{
  printf 'JobID: %s\n' "$SLURM_JOB_ID"
  hostname
  lscpu
  gcc --version
  cmake --version
  uname -a
} | tee results/environment.txt

# README §8.3：正式循环次序实验。
srun --cpu-bind=cores ./build/cache_part3 2048 64 \
  2>&1 | tee results/loop_orders.txt
```

脚本使用 `set -euo pipefail`，实验失败会返回非零，`tee` 不会掩盖错误。
CTest 写法兼容 CMake 3.16 及以上版本。默认加载 `cmake` 模块；模块名称与节点不符时，
按脚本说明通过 `CMAKE_MODULE` 指定实际名称。

同一份 Lab 2 目录一次只运行一个作业。作业结束后再修改源码、重编译或提交下一阶段，
避免争用同一个 `build/` 和覆盖 `results/`。每个阶段默认最多运行 3 小时。
重复测量之前保存上一批日志。

Slurm 总日志位于课程根目录 `course-labs/results/`，README 指定的实验数据文件位于
`course-labs/labs/lab2-matrix/results/`。两者都需要下载。

## 7. 提交与查看状态（登录节点）

若当前终端仍在交互计算节点，先输入 `exit` 返回登录节点。以 Lab 2 为例：

```bash
cd ~/course-labs
mkdir -p results
sbatch lab2.slurm
squeue -u "$USER"
```

Lab 1 使用 `sbatch lab1.slurm`。必须在提交前创建根目录 `results/`，因为 Slurm 会在
脚本开始前打开日志文件。将下面的 `123456` 替换为本次提交返回的作业编号：

```bash
tail -F results/lab2-123456.log
# 按 Ctrl-C 结束日志查看，再执行下一条
sacct -j 123456 --format=JobID,State,ExitCode,Elapsed
```

`tail -F` 会等待日志出现。Ctrl-C 结束查看，不会取消作业。PD 表示排队，R 表示运行。
只有 `COMPLETED`、`ExitCode=0:0` 且实验正确性输出通过，才确认该次作业成功。
确实需要取消时，在登录节点执行 `scancel 123456`。

## 8. 按 README 进行正式性能实验

以下流程对应 [Lab 2 README 第 8、9 节](../labs/lab2-matrix/README.md)。课程已提供各阶段
的完整脚本，以下代码块解释其中的实验命令。提交时在登录节点进入课程根目录，先运行
`mkdir -p results`，然后选择一个阶段：

| 阶段 | 提交命令 |
| --- | --- |
| 小规模正确性检查 | `sbatch slurm/lab2-check.slurm` |
| 寄存器复用 | `sbatch slurm/lab2-register.slurm` |
| 循环次序 | `sbatch lab2.slurm` |
| 块大小筛选 | `sbatch slurm/lab2-block-sweep.slurm` |
| 两个候选值确认 | `sbatch slurm/lab2-block-confirm.slurm 64 128` |
| 四个优化级别 | `sbatch slurm/lab2-opt-levels.slurm 64` |
| O3 重复三次 | `sbatch slurm/lab2-repeat.slurm 64` |

表中 `64 128` 和 `64` 只是示例，必须换成实测候选值或最终最佳块大小。
脚本会自行进入 Lab 2 目录、编译、运行 CTest 并创建实验数据目录。
矩阵规模、块大小候选值和结果文件名与 README 一致；批处理中的程序命令前增加
`srun --cpu-bind=cores` 以绑定分配的 CPU。

若在前面 `srun --pty` 打开的交互式计算终端手动运行，直接使用 README 中不带
`srun --cpu-bind=cores` 前缀的程序命令。该终端已绑定 CPU，无需再嵌套启动 `srun`。
运行前在 Lab 2 目录创建 `results/`，并保存每条命令的正确性结果。

### 8.1 寄存器复用（README §8.2）

寄存器实验要求 `n` 能被 6 整除，正式规模统一为 66、126、258、510、1026、2046，
与 `reg_reuse` 的默认规模及 README 第 9.2 节的报告表格一致。

```bash
srun --cpu-bind=cores ./build/reg_reuse 66 126 258 510 1026 2046 \
  2>&1 | tee results/register_reuse.txt
```

记录 `dgemm0`～`dgemm3` 的正确性、时间和 GFLOP/s。

### 8.2 循环次序（README §8.3）

```bash
srun --cpu-bind=cores ./build/cache_part3 2048 64 \
  2>&1 | tee results/loop_orders.txt
```

记录六种非分块循环次序。该程序也会执行六个分块函数；全部函数需正确实现。

### 8.3 块大小筛选（README §8.4）

固定 `n=1024`，测试 `b=16、32、64、128、256`，记录六个分块函数：

```bash
: > results/cache_block_sweep.txt

for b in 16 32 64 128 256; do
  echo "===== block_size=${b} =====" | tee -a results/cache_block_sweep.txt
  srun --cpu-bind=cores ./build/cache_part3 1024 "$b" \
    2>&1 | tee -a results/cache_block_sweep.txt
done
```

非分块结果与 b 无关，按 README 记录第一次即可。选出两个最佳候选后在 `n=2048` 确认：

```bash
: > results/cache_block_2048.txt

for b in 64 128; do
  echo "===== n=2048, block_size=${b} =====" \
    | tee -a results/cache_block_2048.txt
  srun --cpu-bind=cores ./build/cache_part3 2048 "$b" \
    2>&1 | tee -a results/cache_block_2048.txt
done
```

上面的 64、128 是示例，要换成实际候选值。根据确认结果选择最终块大小。

### 8.4 编译优化级别（README §8.5）

下例假设最终块大小为 64。四个优化级别使用相同的 `n=2048` 和同一个实际最佳块大小：

```bash
for opt in 0 1 2 3; do
  srun --cpu-bind=cores ./build/cache_part4_o${opt} 2048 64 \
    2>&1 | tee "results/optimal_O${opt}.txt"
done
```

### 8.5 重复测量（README §8.6）

每种正式配置建议重复 3 次并取中位数，下面用 O3 举例，块大小仍替换为实际值：

```bash
: > results/optimal_O3_repeated.txt

for run in 1 2 3; do
  echo "===== run=${run} =====" | tee -a results/optimal_O3_repeated.txt
  srun --cpu-bind=cores ./build/cache_part4_o3 2048 64 \
    2>&1 | tee -a results/optimal_O3_repeated.txt
done
```

重复期间保持源码、编译选项、矩阵规模、块大小和 CPU 型号一致。
绑核减少进程迁移，共享节点仍可能存在缓存、带宽和频率干扰。
`tee` 和 `: >` 会覆盖同名文件，重跑前先保存上一批结果。

### 8.6 环境记录和报告（README §9）

每份 Lab 2 Slurm 脚本会自动记录环境，并将记录同时写入总日志。
交互式实验可在实际运行实验的计算节点执行：

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

另外记录 `hostname`、作业编号、各目标编译选项和正确性状态。
使用 Clang 时，将 `gcc --version` 改为 `clang --version`。
报告按 README 第 9 节整理四张表和四类图：寄存器复用、循环次序、缓存块大小、综合优化。
图中注明坐标轴、单位、n 和 b。块大小实验应包含两个候选值在 2048 下的确认结果。

### 8.7 可选的默认批量检查（README §8.7）

全部函数正确后，可以在已申请的计算节点运行：

```bash
bash scripts/run_all.sh mydata.txt
```

结果位于 `labs/lab2-matrix/results/mydata.txt`。此脚本读取相同的 `build/`，使用程序默认参数。
寄存器规模为 66、126、258、510、1026、2046，与正式实验相同；Part 3 的 n/b 为 2000/10，
Part 4 为 2040/60，这两项仍与正式实验参数不同。
它用于额外批量检查，不能替代前面按 README 第 8.2～8.6 节收集的正式数据。
批量脚本在已申请的计算节点运行。使用批处理方式时，返回登录节点并从课程根目录执行：

```bash
mkdir -p results
sbatch slurm/lab2-default-all.slurm
```

## 9. 下载结果（本地电脑）

以下命令适用于 macOS、Linux 和安装了 OpenSSH 的 Windows PowerShell。
使用新的本地 `pi2-results` 目录，分别下载 Slurm 总日志和 Lab 2 实验数据：

```bash
mkdir pi2-results
scp -r jiaowosuan-data:course-labs/results ./pi2-results/slurm
scp -r jiaowosuan-data:course-labs/labs/lab2-matrix/results ./pi2-results/lab2
```

Slurm 日志位于 `pi2-results/slurm/`，实验数据与环境记录位于 `pi2-results/lab2/`。
若目标已存在，先核对版本并换一个带批次名称的新目录，以免混入旧结果。
只运行 Lab 1 时，下载 Slurm 日志即可。关闭 SSH 不会停止已经提交成功的 `sbatch` 作业。

## 10. 通过交大云盘提交作业

先在远程编辑器保存最终代码并完成计算节点上的检查，再回到**本地终端**下载要提交
的源码。下面示例同时收集两个 Lab；正式提交时按各 Lab 题面分别整理。

```bash
mkdir pi2-submission
scp jiaowosuan-data:course-labs/labs/lab1-datalab/bits.c ./pi2-submission/
scp jiaowosuan-data:course-labs/labs/lab2-matrix/src/mygemm.c ./pi2-submission/
```

若目录已存在，先核对版本，或使用带日期的新目录。选择当前 Lab 的源码：
Lab 1 为 bits.c，Lab 2 为 mygemm.c，分别上传对应的“源码提交”任务。
报告、实验数据和要求的日志按课程通知另交。

1. 打开教师发布的云盘收集链接，登录并填写本人学号、姓名。
2. 选择已经自测的本地 .c 源码，提交并等待上传完成。
3. 核对自己的提交记录，保留本地最终文件。
4. 修改后在同一入口重新上传并选择替换旧文件，避免多份最终版本。

云盘按学号与姓名统一命名，例如 0001_示例甲.c。正式评分使用教师下载并归档的源码，
上传本身不会运行测试。多文件实验再按教师要求提交 ZIP。
完整说明和评分反馈的读法见 [作业提交说明](作业提交说明.md)。

自动评分只计算正确性，不计算性能分。缺交、重复、身份信息不符或评分环境故障由助教
核对处理。构建目录、可执行文件、个人 SSH 配置和密钥无需提交。

## 官方参考

- [SSH 登录](https://docs.hpc.sjtu.edu.cn/login/sshlogin.html)
- [免密证书与账号管理](https://docs.hpc.sjtu.edu.cn/accounts/security.html)
- [平台 VS Code 使用说明](https://docs.hpc.sjtu.edu.cn/login/vscode.html)
- [OpenSSH 证书检查命令](https://man.openbsd.org/ssh-keygen)
- [OpenSSH 客户端配置](https://man.openbsd.org/ssh_config)
- [VS Code Remote-SSH](https://code.visualstudio.com/docs/remote/ssh)
- [VS Code 远程连接排错](https://code.visualstudio.com/docs/remote/troubleshooting)
- [文件传输](https://docs.hpc.sjtu.edu.cn/transport/transportsolution.html)
- [Slurm 作业管理](https://docs.hpc.sjtu.edu.cn/job/slurm.html)
- [队列说明](https://docs.hpc.sjtu.edu.cn/job/partition.html)
- [π 2.0 CPU 环境](https://docs.hpc.sjtu.edu.cn/job/kos.html)
- [CMake 3.20 的 CTest --test-dir 支持](https://cmake.org/cmake/help/v3.20/release/3.20.html#ctest)
- [软件模块使用方法](https://docs.hpc.sjtu.edu.cn/app/module.html)

命令依据 2026-09-27 的课程仓库和平台文档整理。队列权限与软件环境以实际账号查询为准。
