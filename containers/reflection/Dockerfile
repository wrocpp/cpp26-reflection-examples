# cpp-reflection: powers every "Reflection today" triptych sub-section.
# FROM the community-maintained vsavkov/clang-p2996 image. We add only
# the small extras readers expect from a wro.cpp container: cmake +
# ninja for project builds, plus the python helper used by the publish
# pipeline.
#
# Why community-maintained: building clang-p2996 from source takes
# 45-90 minutes per CI run. vsavkov/clang-p2996 is a thin pre-built
# layer over the Bloomberg fork. If that image becomes stale or
# unmaintained, we'd switch to a self-built source layer (see the
# top-level Dockerfile in this repo for the source recipe).

FROM vsavkov/clang-p2996:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
        python3 \
        ca-certificates \
        git \
    && rm -rf /var/lib/apt/lists/*

LABEL org.opencontainers.image.source="https://github.com/wrocpp/cpp26-reflection-examples"
LABEL org.opencontainers.image.description="wro.cpp reflection container: clang-p2996 + cmake + ninja. Powers Reflection today demos."
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /work
CMD ["/bin/bash"]
