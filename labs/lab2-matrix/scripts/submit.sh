#!/usr/bin/env bash

# Kept as a compatibility wrapper for old course material. This framework now
# runs directly in the student's Linux environment and does not submit a SLURM
# job. New instructions should call run_all.sh directly.

set -euo pipefail
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
exec "${script_dir}/run_all.sh" "${1:-mydata.txt}"
