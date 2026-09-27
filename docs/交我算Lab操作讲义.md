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
srun -p cpu -N 1 -n 1 -c 1 -t 00:20:00 \
  --cpu-bind=cores --pty /bin/bash -l
hostname
gcc --version
module avail cmake
module load cmake
cmake --version
```

只有资源分配完成后才执行编译和实验。`/bin/bash -l` 启动登录 shell，以初始化
计算节点的软件模块环境。`module load cmake` 加载默认版本，当前框架要求 CMake ≥ 3.16。
若默认模块不可用或版本过旧，从 `module avail cmake` 的结果中选择实际存在的版本，
再用 `module load 完整模块名` 加载并确认版本。官方 KOS 页面列出的示例是
`module load cmake/3.26.3-gcc-8.5.0`，以当前节点的实际查询结果为准。
不要直接在登录节点运行实验。交互调试结束输入 `exit` 释放资源。

## 3. 在计算节点运行 DataLab

```bash
cd ~/course-labs/labs/lab1-datalab
make -j1
make selftest
./btest -f bitCount
make grade
chmod u+x ./dlc
./dlc bits.c
```

先按题面填写 bits.c 的姓名学号，并完成自己的实现。
`selftest` 检查测试框架，显示满分也不代表学生答案正确；`grade` 才检查学生答案。
当前仓库的 `dlc` 没有执行位，首次运行前需执行上面的 `chmod u+x ./dlc`。
它是 x86-64 Linux 程序，只在对应的 Linux 计算节点运行，不在本地 macOS 或 PowerShell 运行。
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
初始学生模板中的函数尚未实现，运行这些检查出现 INVALID 和非零退出码是预期结果。
当前框架已为各测试目标设置相应优化级别，不额外添加全局优化选项。

## 5. DataLab 批处理脚本

将以下内容保存为远端 course-labs/lab1.slurm：

在 VS Code 中使用 UTF-8 编码、LF 换行保存两个 `.slurm` 文件。Windows 的 CRLF
换行会导致 `sbatch` 拒绝脚本；可以通过编辑器右下角的换行格式菜单改为 LF。

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
#!/bin/bash -l
#SBATCH -J matrixlab
#SBATCH -p cpu -N 1 -n 1 -c 1
#SBATCH -t 00:20:00
#SBATCH -o results/lab2-%j.log

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
module load cmake
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
`set -euo pipefail` 会在第一个失败步骤处停止，并使作业返回非零；后续测试可能尚未运行。
脚本自身加载 CMake，不能依赖之前交互计算终端中的环境。模块版本要求与第 2 节一致；
若默认版本不合适，将脚本中的 `module load cmake` 改成已经确认的完整模块名。

## 7. 提交与查看状态（登录节点）

若当前终端仍在交互计算节点，先输入 `exit` 返回登录节点，再执行下面的命令。

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
tail -F results/lab2-123456.log
# 按 Ctrl-C 结束日志查看，再执行下一条
sacct -j 123456 --format=JobID,State,ExitCode,Elapsed
```

`tail -F` 会等待尚未创建的日志文件；用 Ctrl-C 结束查看不会取消作业。
PD 表示排队，R 表示运行。示例日志名和 `sacct` 使用 Lab 2 对应的作业编号。
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

以下命令适用于 macOS、Linux 和安装了 OpenSSH 的 Windows PowerShell：

```bash
mkdir pi2-results
scp -r jiaowosuan-data:course-labs/results ./pi2-results/
```

结果保存在 `pi2-results/results/`。若 `pi2-results` 已存在，先核对版本，或新建一个
带日期的目录并同步替换目标路径。按实验批次保存日志。

macOS / Linux 已安装 rsync 时，也可用以下命令替代上面的 scp；目标父目录须已创建：

```bash
rsync -av \
  jiaowosuan-data:course-labs/results/ \
  ./pi2-results/results/
```

Windows 默认不提供 rsync，使用上面的 scp 即可。关闭 SSH 通常不会停止已提交的 sbatch 作业。

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
- [软件模块使用方法](https://docs.hpc.sjtu.edu.cn/app/module.html)

命令依据 2026-09-27 的课程仓库和平台文档整理。队列权限与软件环境以实际账号查询为准。
