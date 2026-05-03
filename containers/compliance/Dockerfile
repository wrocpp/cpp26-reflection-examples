# cpp-compliance: compliance-cluster `Today` demos.
# What's added on top of cpp-base: cppcheck (with bundled MISRA addon),
# clang-tidy MISRA / AUTOSAR / CERT-C++ checksets (clang-19 already
# carries the engine; the checksets are configured here). No qualified
# vendor compilers (GHS, IAR) -- those are proprietary; the toolset
# page documents how to point your CI at them.

ARG BASE_TAG=2026-05
FROM ghcr.io/wrocpp/cpp-base:${BASE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        cppcheck \
    && rm -rf /var/lib/apt/lists/*

# Ship a default .clang-tidy preset that turns on the MISRA / AUTOSAR /
# CERT-C++ checksets. Containers/scripts/run-misra.sh + run-cert.sh
# point at this file by default.
COPY clang-tidy-misra.yml /opt/wrocpp/clang-tidy-misra.yml
COPY clang-tidy-cert.yml  /opt/wrocpp/clang-tidy-cert.yml
COPY cppcheck-misra.cfg   /opt/wrocpp/cppcheck-misra.cfg

LABEL org.opencontainers.image.source="https://github.com/wrocpp/cpp26-reflection-examples"
LABEL org.opencontainers.image.description="wro.cpp compliance cluster: cppcheck-MISRA, clang-tidy MISRA / AUTOSAR / CERT-C++ presets on cpp-base."
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /work
CMD ["/bin/bash"]
