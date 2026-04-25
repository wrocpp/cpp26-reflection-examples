# Bloomberg clang-p2996 pinned build for arm64 Linux.
# Source: https://github.com/bloomberg/clang-p2996 (branch p2996)
ARG CLANG_P2996_SHA=9ffb96e3ce362289008e14ad2a79a249f58aa90a

############################
# Stage 1: build clang + libc++/libc++abi/libunwind
############################
FROM ubuntu:24.04 AS builder
ARG CLANG_P2996_SHA
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        git \
        cmake \
        ninja-build \
        python3 \
        build-essential \
        lld \
        libzstd-dev \
        zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src/llvm-project
RUN git init -q . \
 && git remote add origin https://github.com/bloomberg/clang-p2996.git \
 && git fetch --depth=1 origin "${CLANG_P2996_SHA}" \
 && git checkout --quiet FETCH_HEAD \
 && git rev-parse HEAD

RUN cmake -S llvm -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/clang-p2996 \
        -DLLVM_ENABLE_PROJECTS="clang" \
        -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
        -DLLVM_TARGETS_TO_BUILD="AArch64" \
        -DLLVM_USE_LINKER=lld \
        -DLLVM_PARALLEL_LINK_JOBS=2 \
        -DLLVM_INCLUDE_TESTS=OFF \
        -DLLVM_INCLUDE_EXAMPLES=OFF \
        -DLLVM_INCLUDE_BENCHMARKS=OFF \
        -DCLANG_INCLUDE_TESTS=OFF

RUN ninja -C build clang
RUN ninja -C build runtimes

# Install clang + runtimes via ninja install-* targets; these proxy through
# to the LLVM_ENABLE_RUNTIMES subbuild, unlike `cmake --install --component`
# at the outer project, which silently misses runtime components.
RUN ninja -C build \
        install-clang \
        install-clang-resource-headers \
        install-cxx \
        install-cxxabi \
        install-unwind

############################
# Stage 2: slim runtime image
############################
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        libc6-dev \
        gcc \
        binutils \
        lld \
        make \
        cmake \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /opt/clang-p2996 /opt/clang-p2996
ENV PATH=/opt/clang-p2996/bin:$PATH
ENV CC=clang CXX=clang++

# Make libc++.so findable by the dynamic linker without per-binary rpath.
RUN echo "/opt/clang-p2996/lib/aarch64-unknown-linux-gnu" > /etc/ld.so.conf.d/clang-p2996.conf \
 && ldconfig

LABEL org.opencontainers.image.source="https://github.com/bloomberg/clang-p2996"
LABEL clang-p2996.pinned-sha="9ffb96e3ce362289008e14ad2a79a249f58aa90a"

WORKDIR /work
CMD ["/bin/bash"]
