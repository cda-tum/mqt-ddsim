#!/usr/bin/env bash

BUILD_DIR='/tmp/cmake-build-hybrid-sim'
BENCH_DIR="hybrid-sim-$(date "+%F-%H-%M-%S")"
TIMEOUT='86400'

benchmark() {
    local OUTFILE="$1"
    shift
    printf "Simulation category %s 'ddsim_simple %s' (%s) ... " "${OUTFILE}" "$*" "$(date "+%F %T")" 1>&2
    {
    timeout --foreground ${TIMEOUT} "${BUILD_DIR}/ddsim_simple" "$@"
    ret=$?
    if [ $ret -ne 0 ]; then
        printf "TIMEOUT (%s) %s\n" "$ret" "$(date "+%F-%H-%M-%S")"
        printf "TIMEOUT (%s) %s\n" "$ret" "$(date "+%F-%H-%M-%S")" 1>&2;
    else
        printf "done (%s)\n" "$(date "+%F %T")"  1>&2
    fi
    } >> "${BENCH_DIR}/${OUTFILE}.log"

}

export BUILD_DIR
export BENCH_DIR
export TIMEOUT
export -f benchmark

mkdir -p "${BENCH_DIR}"
cmake -DCMAKE_BUILD_TYPE=Release -S ~/Code/ddsim/ -B "${BUILD_DIR}" -DBINDINGS=ON || exit 1
cmake --build "${BUILD_DIR}" --config Release --target ddsim_simple || exit 1

START_TIME="$(date "+%F %T")"
printf "Build to %s\n" "${BUILD_DIR}"
printf "Start running all simulations (%s) timeout set to %s\n" "${START_TIME}" "${TIMEOUT}"

for file in "${BUILD_DIR}"/circuits/inst_4x4*.txt; do
  base="$(basename -s .txt "$file")"
  sem -j 4 benchmark hybrid_sim_base_"${base}" --ps --simulate_file "${file}"
  sem -j 1 benchmark hybrid_sim_dd_"${base}" --ps --nthreads 32 --hybrid_mode dd --simulate_file_hybrid "${file}"
  sem -j 1 benchmark hybrid_sim_amp_"${base}" --ps --nthreads 32 --hybrid_mode amplitude --simulate_file_hybrid "${file}"
done

for file in "${BUILD_DIR}"/circuits/inst_4x5*.txt; do
  base="$(basename -s .txt "$file")"
  sem -j 4 benchmark hybrid_sim_base_"${base}" --ps --simulate_file "${file}"
  sem -j 1 benchmark hybrid_sim_dd_"${base}" --ps --nthreads 32 --hybrid_mode dd --simulate_file_hybrid "${file}"
  sem -j 1 benchmark hybrid_sim_amp_"${base}" --ps --nthreads 32 --hybrid_mode amplitude --simulate_file_hybrid "${file}"
done

for file in "${BUILD_DIR}"/circuits/inst_5x5*.txt; do
  base="$(basename -s .txt "$file")"
#  sem -j 2 benchmark hybrid_sim_base_"${base}" --ps --simulate_file "${file}"
#  sem -j 1 benchmark hybrid_sim_dd_"${base}" --ps --nthreads 32 --hybrid_mode dd --simulate_file_hybrid "${file}"
  sem -j 1 benchmark hybrid_sim_amp_"${base}" --ps --nthreads 32 --hybrid_mode amplitude --simulate_file_hybrid "${file}"
done

for file in "${BUILD_DIR}"/circuits/inst_5x6*.txt; do
  base="$(basename -s .txt "$file")"
#  sem -j 2 benchmark hybrid_sim_base_"${base}" --ps --simulate_file "${file}"
#  sem -j 1 benchmark hybrid_sim_dd_"${base}" --ps --nthreads 32 --hybrid_mode dd --simulate_file_hybrid "${file}"
  sem -j 1 benchmark hybrid_sim_amp_"${base}" --ps --nthreads 8 --hybrid_mode amplitude --simulate_file_hybrid "${file}"
done

sem --wait

printf "Finished running all simulations (%s)\n" "$(date "+%F %T")"
printf "(Started running all simulations (%s))\n" "${START_TIME}"
