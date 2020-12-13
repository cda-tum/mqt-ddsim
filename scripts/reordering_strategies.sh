#!/usr/bin/env bash

# strategies:
# - 0 none
# - 1 sifting
# - 2 move2top
# - 3 move2bottom


if [ -z "$1" ]; then
  export BUILD_TYPE='Release'
else
  export BUILD_TYPE="$1"
fi
export BUILD_DIR="/tmp/cmake-build-ddsim-reordering-${BUILD_TYPE,,}"
export TIMEOUT='10h'

START_TIME=$(date +"%Y-%m-%d-%H-%M-%S")
LOG_FILE="reordering_${START_TIME}.log"
COMMAND_FILE="commands_${START_TIME}.par"

run_sim() {
  stype=$1
  circuit=$2
  circuit_bn=$(basename "$circuit")
  initial_strategy=$3
  dynamic_strategy=$4
  post_strategy=$5
  run_start_time=$(date +"%Y-%m-%d-%H-%M-%S")
  printf "%s %s, %s, R%s%s%s, " "${stype:11}" "${circuit_bn}" "${BUILD_TYPE}" "${initial_strategy}" "${dynamic_strategy}" "${post_strategy}"
  timeout --foreground ${TIMEOUT} "${BUILD_DIR}/apps/ddsim_simple" --benchmark "${stype}" "${circuit}" --initial_reorder "${initial_strategy}" --dynamic_reorder "${dynamic_strategy}" --post_reorder "${post_strategy}" ||
    printf "%s, X, Error %s (%s), Start=%s, End=%s\n" "${circuit_bn}" "$?" "${TIMEOUT}" "${run_start_time}" "$(date +"%Y-%m-%d %H:%M:%S")"
}

add_benchmark() {
  stype=$1
  shift
  parallel --dry-run run_sim ::: "${stype}" ::: "$@" ::: 0 0 1 1 2 2 0 :::+ 0 2 0 2 0 2 0 :::+ 0 0 0 0 0 0 1 >> "${COMMAND_FILE}"
}

echo "(Re)Building in ${BUILD_DIR} (${BUILD_TYPE}) as necessary"
cmake -DGIT_SUBMODULE=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -S .. -B "${BUILD_DIR}" >/dev/null || exit 1
cmake --build "${BUILD_DIR}" --config ${BUILD_TYPE} --target ddsim_simple >/dev/null || exit 1

echo "Building benchmark"
add_benchmark --simulate_qft 11 14 17 20 23
add_benchmark --simulate_file ../circuits/inst_4x5_1[01]_[01].txt
add_benchmark --simulate_file ../circuits/inst_4x4_1[0123456]_[01].txt
add_benchmark --simulate_grover  11 14 17 20 22 23

# logged output starts here

if command -v lshw &>/dev/null; then
  lshw -short -C memory -C processor -C storage -quiet 2>/dev/null > "${LOG_FILE}"
fi

#ulimit -v $(( 20*1024*1024 )) # limit memory to 20 GiB
printf "%s has %s lines\n" "${COMMAND_FILE}" "$(wc -l "${COMMAND_FILE}")" | tee -a "${LOG_FILE}"
printf "ulimit -v is set to %s (kb)\n" "$(ulimit -v)" | tee -a "${LOG_FILE}"
printf "timeout is set to %s\n" "${TIMEOUT}" | tee -a "${LOG_FILE}"
echo "Start time: ${START_TIME}" | tee -a "${LOG_FILE}"
printf "benchmark, buildtype, reordering, " | tee -a "${LOG_FILE}"
printf "name, qubits, simtime, rootsize, reordering, reordercount, exchangecount, substitutions, collsions, S, smin, smax, " | tee -a "${LOG_FILE}"
printf "UT, utlookups, utcollisions, utmatches, uthitrate, " | tee -a "${LOG_FILE}"
printf "GC, gccalls, gcruns, seed, ops, maxnodecount\n" | tee -a "${LOG_FILE}"

export -f run_sim
parallel -j4 -n1 -k :::: "${COMMAND_FILE}"

echo "Start time: ${START_TIME}" | tee -a "${LOG_FILE}"
echo "End time: $(date +"%Y-%m-%d %H:%M:%S")" | tee -a "${LOG_FILE}"
