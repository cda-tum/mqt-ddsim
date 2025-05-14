#!/usr/bin/env bash
# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

BUILD_DIR='/tmp/cmake-build-ddsim-primebases'

echo "(Re)Building in ${BUILD_DIR} as necessary"
cmake -DCMAKE_BUILD_TYPE=Release -S .. -B "${BUILD_DIR}" > /dev/null || exit 1
cmake --build "${BUILD_DIR}" --config Release --target ddsim_primebases ddsim_simple  > /dev/null || exit 1


echo "Start time: $(date +"%Y-%m-%d %H:%M:%S")"
for base in $("${BUILD_DIR}/apps/ddsim_primebases" -N 749 -S primes -L 25); do
  echo "Base $base"
  "${BUILD_DIR}/apps/ddsim_simple" --simulate_shor 749 --simulate_shor_coprime "$base" --benchmark --verbose
done
echo "End time: $(date +"%Y-%m-%d %H:%M:%S")"
