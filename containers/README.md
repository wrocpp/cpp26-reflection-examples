# wro.cpp toolset containers

Pre-built Docker images that back the "Reproduce locally" affordances
on every wro.cpp toolset page (https://wrocpp.github.io/toolset/).

## Image map

| Image | Purpose | Base |
|---|---|---|
| `ghcr.io/wrocpp/cpp-base` | Shared substrate. Ubuntu 24.04 + clang-19 + GCC 14 + CMake + Ninja + Python3. | `ubuntu:24.04` |
| `ghcr.io/wrocpp/cpp-safety` | Safety-cluster `Today` demos: sanitizers (ASan/UBSan/TSan/MSan/HWASan), AFL++, libFuzzer, clang-tidy. | `cpp-base` |
| `ghcr.io/wrocpp/cpp-performance` | Performance-cluster `Today` demos: perf, Tracy server + capture, Perfetto SDK, google-benchmark. | `cpp-base` |
| `ghcr.io/wrocpp/cpp-security` | Security-cluster `Today` demos: vcpkg, Conan, syft (SBOM), grype (CVE scan), libsodium-dev, OpenSSL dev headers. | `cpp-base` |
| `ghcr.io/wrocpp/cpp-compliance` | Compliance-cluster `Today` demos: cppcheck-MISRA, clang-tidy MISRA / AUTOSAR / CERT-C++ check sets. | `cpp-base` |
| `ghcr.io/wrocpp/cpp-reflection` | Powers every `Reflection today` triptych sub-section across all clusters. clang-p2996 fork ready to use. | `vsavkov/clang-p2996` (community-maintained) |

## Tag scheme

- `2026-05` -- rolling release. What the toolset pages link to.
- `2026-05-DD` -- quarterly immutable snapshot. Pinned in
  `containers/scripts/*.sh` for reproducibility.
- `sha-<git-sha>` -- per-build snapshot. Debugging only.
- `branch-<name>` -- per-feature-branch builds for PR review.

## Pull

```bash
docker pull ghcr.io/wrocpp/cpp-base:2026-05
docker pull ghcr.io/wrocpp/cpp-safety:2026-05
docker pull ghcr.io/wrocpp/cpp-performance:2026-05
docker pull ghcr.io/wrocpp/cpp-security:2026-05
docker pull ghcr.io/wrocpp/cpp-compliance:2026-05
docker pull ghcr.io/wrocpp/cpp-reflection:2026-05
```

All images are public; no `docker login` required.

## Run

The standard pattern is to mount the current directory and pick the
right image for the task:

```bash
docker run --rm -it \
  -v "$PWD":/work -w /work \
  ghcr.io/wrocpp/cpp-safety:2026-05 \
  containers/scripts/run-asan.sh examples/use-after-free.cpp
```

Each toolset page (https://wrocpp.github.io/toolset/) carries an
embedded `docker run` block under "Reproduce locally". Copy + paste.

## Build local (for container development)

GitHub Actions builds + pushes to GHCR on every push to `main` (and
on any push touching `containers/`). To iterate locally:

```bash
docker buildx build --load -t cpp-base:local containers/base
docker buildx build --load -t cpp-safety:local --build-arg BASE=cpp-base:local containers/safety
# ... etc
```

See `.github/workflows/build-containers.yml` for the canonical build
recipe.

## Why two compiler families?

- **clang-19 + GCC 14 in cpp-base** -- the cluster containers (safety,
  performance, security, compliance) deal with sanitizers, profilers,
  supply chain, MISRA checkers. None of these need C++26 reflection.
  Apt-installable compilers keep the build fast.
- **clang-p2996 in cpp-reflection** -- every triptych sub-section's
  `Reflection today` demo uses the Bloomberg fork. We pull it as a
  community-maintained pre-built (`vsavkov/clang-p2996`) instead of
  building it ourselves; building from source takes 45-90 minutes per
  CI run.

If you need GCC 16.1 (released April 2026, ships C++26 reflection),
godbolt's `g161` toolchain has it. We do not bake it into a container
yet; once it lands in `apt` for Ubuntu LTS, we'll add it to cpp-base.

## Scripts

`containers/scripts/run-*.sh` are tiny wrappers that the toolset
DockerRun blocks invoke. They keep the per-page command short.
