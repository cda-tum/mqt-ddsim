#!/usr/bin/env bash

BUILD_DIR='/tmp/cmake-build-approx'
BENCH_DIR="approx-$(date "+%F-%H-%M-%S")"
TIMEOUT='1h'

benchmark() {
    local OUTFILE="$1"
    shift
    printf "Starting simulation %s '%s' (%s)\n" "${OUTFILE}" "$*" "$(date "+%F %T")"
    #echo timeout ${TIMEOUT} "${BUILD_DIR}/ddsim_simple" "$@"
    { timeout ${TIMEOUT} "${BUILD_DIR}/ddsim_simple" "$@" || { printf "TIMEOUT (%s)\n" "$?"; printf "   TIMEOUT\n" 1>&2; } } > "${BENCH_DIR}/${OUTFILE}.log"
}


mkdir -p "${BENCH_DIR}"
cmake -DCMAKE_BUILD_TYPE=Release -S .. -B "${BUILD_DIR}" || exit 1
cmake --build "${BUILD_DIR}" --config Release --target ddsim_simple || exit 1

START_TIME="$(date "+%F %T")"
printf "Build to %s and timeout set to %s" "${BUILD_DIR}" "${TIMEOUT}"
printf "Start running all simulations (%s)\n" "${START_TIME}"

benchmark state-inst-4x4-10-0 --approx_state --simulate_file circuits/inst_4x4_10_0.txt
benchmark state-inst-4x4-10-1 --approx_state --simulate_file circuits/inst_4x4_10_1.txt
benchmark state-inst-4x4-11-0 --approx_state --simulate_file circuits/inst_4x4_11_0.txt
benchmark state-inst-4x4-11-1 --approx_state --simulate_file circuits/inst_4x4_11_1.txt
benchmark state-inst-4x4-12-0 --approx_state --simulate_file circuits/inst_4x4_12_0.txt
benchmark state-inst-4x4-12-1 --approx_state --simulate_file circuits/inst_4x4_12_1.txt
benchmark state-inst-4x4-13-0 --approx_state --simulate_file circuits/inst_4x4_13_0.txt
benchmark state-inst-4x4-13-1 --approx_state --simulate_file circuits/inst_4x4_13_1.txt
benchmark state-inst-4x4-14-0 --approx_state --simulate_file circuits/inst_4x4_14_0.txt
benchmark state-inst-4x4-14-1 --approx_state --simulate_file circuits/inst_4x4_14_1.txt
benchmark state-inst-4x4-15-0 --approx_state --simulate_file circuits/inst_4x4_15_0.txt
benchmark state-inst-4x4-15-1 --approx_state --simulate_file circuits/inst_4x4_15_1.txt

benchmark state-inst-4x5-10-0 --approx_state --simulate_file circuits/inst_4x5_10_0.txt
benchmark state-inst-4x5-10-1 --approx_state --simulate_file circuits/inst_4x5_10_1.txt

benchmark state-shor-033-2 --approx_state --simulate_shor 33 --simulate_shor_coprime 2
benchmark state-shor-033-2 --approx_state --simulate_shor 33 --simulate_shor_coprime 5
benchmark state-shor-055-2 --approx_state --simulate_shor 55 --simulate_shor_coprime 2
benchmark state-shor-069-4 --approx_state --simulate_shor 69 --simulate_shor_coprime 4
benchmark state-shor-221-4 --approx_state --simulate_shor 221 --simulate_shor_coprime 4
benchmark state-shor-323-8 --approx_state --simulate_shor 323 --simulate_shor_coprime 8


benchmark simmem-inst-4x4-10-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_10_0.txt
benchmark simmem-inst-4x4-10-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_10_0.txt
benchmark simmem-inst-4x4-10-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_10_0.txt
benchmark simmem-inst-4x4-10-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_10_0.txt

benchmark simmem-inst-4x4-10-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_10_1.txt
benchmark simmem-inst-4x4-10-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_10_1.txt
benchmark simmem-inst-4x4-10-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_10_1.txt
benchmark simmem-inst-4x4-10-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_10_1.txt

benchmark simmem-inst-4x4-11-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_11_0.txt
benchmark simmem-inst-4x4-11-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_11_0.txt
benchmark simmem-inst-4x4-11-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_11_0.txt
benchmark simmem-inst-4x4-11-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_11_0.txt

benchmark simmem-inst-4x4-11-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_11_1.txt
benchmark simmem-inst-4x4-11-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_11_1.txt
benchmark simmem-inst-4x4-11-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_11_1.txt
benchmark simmem-inst-4x4-11-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_11_1.txt

benchmark simmem-inst-4x4-12-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_12_0.txt
benchmark simmem-inst-4x4-12-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_12_0.txt
benchmark simmem-inst-4x4-12-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_12_0.txt
benchmark simmem-inst-4x4-12-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_12_0.txt

benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_12_1.txt
benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_12_1.txt
benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_12_1.txt
benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_12_1.txt

benchmark simmem-inst-4x4-13-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_13_0.txt
benchmark simmem-inst-4x4-13-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_13_0.txt
benchmark simmem-inst-4x4-13-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_13_0.txt
benchmark simmem-inst-4x4-13-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_13_0.txt

benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_13_1.txt
benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_13_1.txt
benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_13_1.txt
benchmark simmem-inst-4x4-12-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_13_1.txt

benchmark simmem-inst-4x4-14-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_14_0.txt
benchmark simmem-inst-4x4-14-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_14_0.txt
benchmark simmem-inst-4x4-14-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_14_0.txt
benchmark simmem-inst-4x4-14-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_14_0.txt

benchmark simmem-inst-4x4-14-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_14_1.txt
benchmark simmem-inst-4x4-14-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_14_1.txt
benchmark simmem-inst-4x4-14-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_14_1.txt
benchmark simmem-inst-4x4-14-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_14_1.txt

benchmark simmem-inst-4x4-15-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_15_0.txt
benchmark simmem-inst-4x4-15-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_15_0.txt
benchmark simmem-inst-4x4-15-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_15_0.txt
benchmark simmem-inst-4x4-15-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_15_0.txt

benchmark simmem-inst-4x4-15-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x4_15_1.txt
benchmark simmem-inst-4x4-15-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x4_15_1.txt
benchmark simmem-inst-4x4-15-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x4_15_1.txt
benchmark simmem-inst-4x4-15-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x4_15_1.txt

benchmark simmem-inst-4x5-10-0 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x5_10_0.txt
benchmark simmem-inst-4x5-10-0 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x5_10_0.txt
benchmark simmem-inst-4x5-10-0 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x5_10_0.txt
benchmark simmem-inst-4x5-10-0 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x5_10_0.txt

benchmark simmem-inst-4x5-10-1 --ps --approx_when memory --step_fidelity 1.000 --simulate_file circuits/inst_4x5_10_1.txt
benchmark simmem-inst-4x5-10-1 --ps --approx_when memory --step_fidelity 0.990 --simulate_file circuits/inst_4x5_10_1.txt
benchmark simmem-inst-4x5-10-1 --ps --approx_when memory --step_fidelity 0.975 --simulate_file circuits/inst_4x5_10_1.txt
benchmark simmem-inst-4x5-10-1 --ps --approx_when memory --step_fidelity 0.950 --simulate_file circuits/inst_4x5_10_1.txt


benchmark simfid-shor-33-5 --ps --step_fidelity 0.9 --simulate_shor 33 --simulate_shor_coprime 5
benchmark simfid-shor-55-2 --ps --step_fidelity 0.9 --simulate_shor 55 --simulate_shor_coprime 2
benchmark simfid-shor-69-2 --ps --step_fidelity 0.9 --simulate_shor 69 --simulate_shor_coprime 2
benchmark simfid-shor-221-4 --ps --step_fidelity 0.9 --simulate_shor 221 --simulate_shor_coprime 4
benchmark simfid-shor-323-8 --ps --step_fidelity 0.9 --simulate_shor 323 --simulate_shor_coprime 8
benchmark simfid-shor-629-8 --ps --step_fidelity 0.9 --simulate_shor 629 --simulate_shor_coprime 8
benchmark simfid-shor-1157-8 --ps --step_fidelity 0.9 --simulate_shor 1157 --simulate_shor_coprime 8


printf "Finished running all simulations (%s)\n" "$(date "+%F %T")"
printf "(Started running all simulations (%s))\n" "${START_TIME}"