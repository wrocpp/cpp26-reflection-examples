# cpp-security: security-cluster `Today` demos.
# What's added on top of cpp-base: OpenSSL + libsodium dev headers,
# Conan (pip), syft + grype (curl installers from Anchore), vcpkg
# (git clone + bootstrap). Heavyweight tools intentionally fetched
# at image-build time so reader pulls are quick.

ARG BASE_TAG=2026-05
FROM ghcr.io/wrocpp/cpp-base:${BASE_TAG}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        libssl-dev \
        libsodium-dev \
        zip \
        unzip \
        tar \
        pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Conan via pip (not apt; apt version lags).
RUN pip3 install --break-system-packages --no-cache-dir conan

# syft + grype from Anchore. Pinned via the install script's CHECK_SHA
# pattern when published; for now pull latest stable.
RUN curl -sSfL https://raw.githubusercontent.com/anchore/syft/main/install.sh | sh -s -- -b /usr/local/bin \
 && curl -sSfL https://raw.githubusercontent.com/anchore/grype/main/install.sh | sh -s -- -b /usr/local/bin

# vcpkg: clone shallow + bootstrap. Installed under /opt so it survives
# bind-mounted /work. Reader sets VCPKG_ROOT or runs /opt/vcpkg/vcpkg.
RUN git clone --depth=1 https://github.com/microsoft/vcpkg.git /opt/vcpkg \
 && /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics
ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH=/opt/vcpkg:$PATH

LABEL org.opencontainers.image.source="https://github.com/wrocpp/cpp26-reflection-examples"
LABEL org.opencontainers.image.description="wro.cpp security cluster: vcpkg, Conan, syft, grype, libsodium, OpenSSL on cpp-base."
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /work
CMD ["/bin/bash"]
