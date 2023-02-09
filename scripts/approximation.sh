#!/usr/bin/env bash

BUILD_DIR='/tmp/cmake-build-approx'
BENCH_DIR="approx-$(date "+%F-%H-%M-%S")"
TIMEOUT='0'

benchmark() {
    local OUTFILE="$1"
    shift
    printf "Simulation category %s 'ddsim_simple %s' (%s) ... " "${OUTFILE}" "$*" "$(date "+%F %T")" 1>&2
    #echo timeout ${TIMEOUT} "${BUILD_DIR}/ddsim_simple" "$@"
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
cmake -DCMAKE_BUILD_TYPE=Release -S .. -B "${BUILD_DIR}" || exit 1
cmake --build "${BUILD_DIR}" --config Release --target ddsim_simple || exit 1

START_TIME="$(date "+%F %T")"
printf "Build to %s\n" "${BUILD_DIR}"
printf "Start running all simulations (%s) timeout set to %s\n" "${START_TIME}" "${TIMEOUT}"

sem -j 4 benchmark state-inst-4x4-10-0 --approx_state --simulate_file circuits/inst_4x4_10_0.txt
sem -j 4 benchmark state-inst-4x4-10-1 --approx_state --simulate_file circuits/inst_4x4_10_1.txt
sem -j 4 benchmark state-inst-4x4-11-0 --approx_state --simulate_file circuits/inst_4x4_11_0.txt
sem -j 4 benchmark state-inst-4x4-11-1 --approx_state --simulate_file circuits/inst_4x4_11_1.txt
sem -j 4 benchmark state-inst-4x4-12-0 --approx_state --simulate_file circuits/inst_4x4_12_0.txt
sem -j 4 benchmark state-inst-4x4-12-1 --approx_state --simulate_file circuits/inst_4x4_12_1.txt
sem -j 4 benchmark state-inst-4x4-13-0 --approx_state --simulate_file circuits/inst_4x4_13_0.txt
sem -j 4 benchmark state-inst-4x4-13-1 --approx_state --simulate_file circuits/inst_4x4_13_1.txt
sem -j 4 benchmark state-inst-4x4-14-0 --approx_state --simulate_file circuits/inst_4x4_14_0.txt
sem -j 4 benchmark state-inst-4x4-14-1 --approx_state --simulate_file circuits/inst_4x4_14_1.txt
sem -j 4 benchmark state-inst-4x4-15-0 --approx_state --simulate_file circuits/inst_4x4_15_0.txt
sem -j 4 benchmark state-inst-4x4-15-1 --approx_state --simulate_file circuits/inst_4x4_15_1.txt

sem -j 3 benchmark state-inst-4x5-10-0 --approx_state --simulate_file circuits/inst_4x5_10_0.txt
sem -j 3 benchmark state-inst-4x5-10-1 --approx_state --simulate_file circuits/inst_4x5_10_1.txt
sem -j 3 benchmark state-inst-4x5-11-0 --approx_state --simulate_file circuits/inst_4x5_11_0.txt
sem -j 3 benchmark state-inst-4x5-11-1 --approx_state --simulate_file circuits/inst_4x5_11_1.txt
sem -j 3 benchmark state-inst-4x5-12-0 --approx_state --simulate_file circuits/inst_4x5_12_0.txt
sem -j 3 benchmark state-inst-4x5-12-1 --approx_state --simulate_file circuits/inst_4x5_12_1.txt
sem -j 3 benchmark state-inst-4x5-13-0 --approx_state --simulate_file circuits/inst_4x5_13_0.txt
sem -j 3 benchmark state-inst-4x5-13-1 --approx_state --simulate_file circuits/inst_4x5_13_1.txt
sem -j 3 benchmark state-inst-4x5-14-0 --approx_state --simulate_file circuits/inst_4x5_14_0.txt
sem -j 3 benchmark state-inst-4x5-14-1 --approx_state --simulate_file circuits/inst_4x5_14_1.txt
sem -j 3 benchmark state-inst-4x5-15-0 --approx_state --simulate_file circuits/inst_4x5_15_0.txt
sem -j 3 benchmark state-inst-4x5-15-1 --approx_state --simulate_file circuits/inst_4x5_15_1.txt


sem -j 2 benchmark state-shor-629-8 --approx_state --simulate_shor 629 --simulate_shor_coprime 8
sem -j 2 benchmark state-shor-323-8 --approx_state --simulate_shor 323 --simulate_shor_coprime 8
sem -j 2 benchmark state-shor-221-4 --approx_state --simulate_shor 221 --simulate_shor_coprime 4
sem -j 2 benchmark state-shor-069-4 --approx_state --simulate_shor 69 --simulate_shor_coprime 4
sem -j 2 benchmark state-shor-055-2 --approx_state --simulate_shor 55 --simulate_shor_coprime 2
sem -j 2 benchmark state-shor-033-2 --approx_state --simulate_shor 33 --simulate_shor_coprime 5

for file in ./circuits/inst_4*.txt; do
  base="$(basename -s .txt "$file")"
  sem -j 3 benchmark simmem000"${base}" --ps --approx_when memory --step_fidelity 1.000 --simulate_file "${file}"
  sem -j 3 benchmark simmem990"${base}" --ps --approx_when memory --step_fidelity 0.990 --simulate_file "${file}"
  sem -j 3 benchmark simmem975"${base}" --ps --approx_when memory --step_fidelity 0.975 --simulate_file "${file}"
  sem -j 3 benchmark simmem950"${base}" --ps --approx_when memory --step_fidelity 0.950 --simulate_file "${file}"
  sem -j 3 benchmark simmem925"${base}" --ps --approx_when memory --step_fidelity 0.925 --simulate_file "${file}"
done

sem -j 2 benchmark simfid629 --ps --step_fidelity 1.0 --simulate_shor 629 --simulate_shor_coprime 8
sem -j 2 benchmark simfid323 --ps --step_fidelity 1.0 --simulate_shor 323 --simulate_shor_coprime 8
sem -j 2 benchmark simfid221 --ps --step_fidelity 1.0 --simulate_shor 221 --simulate_shor_coprime 4
sem -j 2 benchmark simfid69 --ps --step_fidelity 1.0 --simulate_shor 69 --simulate_shor_coprime 2
sem -j 2 benchmark simfid55 --ps --step_fidelity 1.0 --simulate_shor 55 --simulate_shor_coprime 2
sem -j 2 benchmark simfid33 --ps --step_fidelity 1.0 --simulate_shor 33 --simulate_shor_coprime 5

sem -j 2 benchmark simfid629 --ps --step_fidelity 0.9 --simulate_shor 629 --simulate_shor_coprime 8
sem -j 2 benchmark simfid323 --ps --step_fidelity 0.9 --simulate_shor 323 --simulate_shor_coprime 8
sem -j 2 benchmark simfid221 --ps --step_fidelity 0.9 --simulate_shor 221 --simulate_shor_coprime 4
sem -j 2 benchmark simfid69 --ps --step_fidelity 0.9 --simulate_shor 69 --simulate_shor_coprime 2
sem -j 2 benchmark simfid55 --ps --step_fidelity 0.9 --simulate_shor 55 --simulate_shor_coprime 2
sem -j 2 benchmark simfid33 --ps --step_fidelity 0.9 --simulate_shor 33 --simulate_shor_coprime 5

sem -j 2 benchmark state-shor-1157-8 --approx_state --simulate_shor 1157 --simulate_shor_coprime 8

sem -j 2 benchmark simfid1157 --ps --step_fidelity 0.9 --simulate_shor 1157 --simulate_shor_coprime 8
sem -j 2 benchmark simfid1157 --ps --step_fidelity 1.0 --simulate_shor 1157 --simulate_shor_coprime 8

sem --wait

printf "Finished running all simulations (%s)\n" "$(date "+%F %T")"
printf "(Started running all simulations (%s))\n" "${START_TIME}"

# post processing
# jq --raw-output '.statistics | [.benchmark, .n_qubits, .max_nodes, .simulation_time, (.approximation_runs, .step_fidelity, .final_fidelity | tonumber)] | @csv'
