# cpp-base: shared substrate for the wro.cpp toolset cluster containers.
#
# What's in here: Ubuntu 24.04 + apt clang-19 + apt GCC 14 (current LTS
# default) + CMake + Ninja + Python3. Designed to be small and fast to
# build (<5 min on GH Actions) so cluster images that FROM cpp-base
# stay equally cheap.
#
# What's NOT in here: clang-p2996 (reflection -- that lives in
# cpp-reflection, FROM vsavkov/clang-p2996). The toolset cluster pages
# (safety, performance, security, compliance) do not need P2996 for
# their "Today" demos; reflection-augmented examples use cpp-reflection
# instead.
#
# See containers/README.md for the full image map.

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        clang-19 \
        clang++-19 \
        clang-tools-19 \
        lld-19 \
        libc++-19-dev \
        libc++abi-19-dev \
        gcc \
        g++ \
        cmake \
        ninja-build \
        make \
        git \
        python3 \
        python3-pip \
        curl \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Symlink versioned clang to bare names so user examples just work.
RUN ln -sf /usr/bin/clang-19 /usr/local/bin/clang \
 && ln -sf /usr/bin/clang++-19 /usr/local/bin/clang++ \
 && ln -sf /usr/bin/clang-tidy-19 /usr/local/bin/clang-tidy \
 && ln -sf /usr/bin/clang-format-19 /usr/local/bin/clang-format \
 && ln -sf /usr/bin/lld-19 /usr/local/bin/lld

LABEL org.opencontainers.image.source="https://github.com/wrocpp/cpp26-reflection-examples"
LABEL org.opencontainers.image.description="wro.cpp shared substrate: Ubuntu 24.04 + clang-19 + GCC 14 + CMake + Ninja"
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /work
CMD ["/bin/bash"]
