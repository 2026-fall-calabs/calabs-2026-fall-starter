#!/bin/bash -l
#SBATCH -J datalab
#SBATCH -p cpu -N 1 -n 1 -c 1
#SBATCH -t 03:00:00
#SBATCH -o results/lab1-%j.log

set -euo pipefail
: "${SLURM_JOB_ID:?请使用 sbatch 提交作业，不要在登录节点直接运行}"

if (( $# > 1 )); then
  echo "用法：sbatch slurm/lab1-10.slurm [函数名]" >&2
  exit 2
fi
cd "${SLURM_SUBMIT_DIR:?请从课程根目录提交}/labs/lab1-datalab"
make -j1
if (( $# == 1 )); then
  srun --cpu-bind=cores ./btest -f "$1"
else
  srun --cpu-bind=cores ./btest -g
fi
