#!/usr/bin/env bash

set -uo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(cd -- "${script_dir}/.." && pwd)"
build_dir="${project_dir}/build"
output_name="${1:-mydata.txt}"
output_path="${project_dir}/results/${output_name}"
overall_status=0

mkdir -p "${project_dir}/results"

required=(
  reg_reuse
  cache_part3
  cache_part4_o0
  cache_part4_o1
  cache_part4_o2
  cache_part4_o3
)

for executable in "${required[@]}"; do
  if [[ ! -x "${build_dir}/${executable}" ]]; then
    echo "Missing ${build_dir}/${executable}. Build the framework first:" >&2
    echo "  cmake -S ${project_dir} -B ${build_dir}" >&2
    echo "  cmake --build ${build_dir} -j" >&2
    exit 2
  fi
done

exec > >(tee "${output_path}") 2>&1

run_test() {
  local title="$1"
  shift
  echo
  echo "*********** ${title} ***********"
  if ! "$@"; then
    overall_status=1
  fi
}

run_test "Register reuse" "${build_dir}/reg_reuse"
run_test "Cache reuse - Part 3" "${build_dir}/cache_part3"
run_test "Cache reuse - Part 4, -O0" "${build_dir}/cache_part4_o0"
run_test "Cache reuse - Part 4, -O1" "${build_dir}/cache_part4_o1"
run_test "Cache reuse - Part 4, -O2" "${build_dir}/cache_part4_o2"
run_test "Cache reuse - Part 4, -O3" "${build_dir}/cache_part4_o3"

echo
echo "Results saved to ${output_path}"
exit "${overall_status}"
