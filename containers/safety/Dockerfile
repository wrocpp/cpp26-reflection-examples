# cpp-safety: safety-cluster `Today` demos.
# What's added on top of cpp-base: AFL++ fuzzer, libFuzzer integration
# helpers, libstdc++ debug variant, additional sanitizer runtime symbols.
# (clang-tidy + sanitizers themselves already ship in cpp-base via
# clang-19.)

ARG BASE_TAG=2026-05
FROM ghcr.io/wrocpp/cpp-base:${BASE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        afl++ \
        libstdc++-13-dev \
        valgrind \
    && rm -rf /var/lib/apt/lists/*

LABEL org.opencontainers.image.source="https://github.com/wrocpp/cpp26-reflection-examples"
LABEL org.opencontainers.image.description="wro.cpp safety cluster: AFL++, libFuzzer, sanitizers, valgrind on cpp-base."
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /work
CMD ["/bin/bash"]
