#!/usr/bin/env bash
# Build the clang-p2996 Docker image natively on arm64.
set -euo pipefail

SHA=9ffb96e3ce362289008e14ad2a79a249f58aa90a
SHORT=${SHA:0:12}
IMAGE=clang-p2996:${SHORT}

cd "$(dirname "$0")"

echo "Building ${IMAGE} (expect 45-90 min on an 8-10 core arm64 VM)..."
docker build \
    --platform linux/arm64 \
    --build-arg "CLANG_P2996_SHA=${SHA}" \
    --tag "${IMAGE}" \
    --tag clang-p2996:latest \
    .

echo
echo "Built ${IMAGE}"
echo "Smoke test:"
echo "  ./cpp hello_reflection.cpp -o hello_reflection && ./run ./hello_reflection"
