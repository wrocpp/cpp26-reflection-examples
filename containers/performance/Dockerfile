# cpp-performance: performance-cluster `Today` demos.
# What's added on top of cpp-base: linux-tools (perf), google-benchmark
# headers + library, hyperfine for quick microbenchmarks. Tracy server
# and Perfetto SDK are NOT bundled (they involve heavy GUI/build deps);
# the toolset page links the upstream projects + ships a tiny
# `containers/scripts/run-perf.sh` for the apt-installable `perf`.

ARG BASE_TAG=2026-05
FROM ghcr.io/wrocpp/cpp-base:${BASE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        linux-tools-generic \
        libbenchmark-dev \
        hyperfine \
    && rm -rf /var/lib/apt/lists/*

# Ensure `perf` is on PATH (linux-tools-generic installs as
# /usr/lib/linux-tools/<kver>/perf and symlinks via update-alternatives).
RUN ln -sf /usr/lib/linux-tools-generic/perf /usr/local/bin/perf 2>/dev/null || true

LABEL org.opencontainers.image.source="https://github.com/wrocpp/cpp26-reflection-examples"
LABEL org.opencontainers.image.description="wro.cpp performance cluster: perf, google-benchmark, hyperfine on cpp-base."
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /work
CMD ["/bin/bash"]
