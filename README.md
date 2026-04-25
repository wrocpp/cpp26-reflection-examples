# C++26 Reflection — hands-on blog series

A runnable, example-driven walkthrough of C++26 static reflection (P2996 and friends), built on a pinned `bloomberg/clang-p2996` Docker image.

Every post has working code you can compile and run in under a minute. Every example is also mirrored on Compiler Explorer so you can play without installing anything.

---

## Try it now

### Option A — Compiler Explorer (zero install)

Each post's `examples/godbolt.md` lists permalinks. Click, edit, share.

### Option B — Local container (arm64 or x86_64 Linux/macOS)

Requires Docker (or Colima/OrbStack on macOS). Build once, then use the wrappers.

```sh
./build.sh                              # 30-60 min, one-time; arm64-native
./cpp hello_reflection.cpp -o hello     # compile with clang-p2996
./run ./hello                           # run inside the container
```

**Toolchain:** pinned to [`bloomberg/clang-p2996@9ffb96e3ce36`](https://github.com/bloomberg/clang-p2996/tree/9ffb96e3ce362289008e14ad2a79a249f58aa90a). The Dockerfile builds LLVM for `AArch64` only with minimal components to keep build time and image size reasonable.

All examples compile with:
```
clang++ -std=c++26 -freflection -stdlib=libc++ ...
```
which is what `./cpp` wraps.

---

## The series

**Status legend:** ⬜ planned · 🚧 drafting · ✅ published

### Arc 1 — Foundations

| # | Post | Audience | Status |
|---|------|----------|--------|
| 1 | [Why C++26 reflection matters](posts/01-why-it-matters/) | Polyglot | ⬜ |
| 2 | [Your first `^^`: reflecting types, walking members](posts/02-first-reflection/) | Mixed | ⬜ |
| 3 | [Splicing: `[: r :]` and round-tripping reflections](posts/03-splicing/) | Working C++ | ⬜ |
| 4 | [`template for`: iterating reflections at compile time](posts/04-expansion-statements/) | Working C++ | ⬜ |

### Arc 2 — Everyday wins (`lib/reflect_print`)

| # | Post | Audience | Status |
|---|------|----------|--------|
| 5 | [Goodbye `magic_enum`: enum reflection done right](posts/05-enum-to-string/) | Working C++ | ⬜ |
| 6 | [Auto-generating `std::formatter<T>` for any aggregate](posts/06-auto-formatter/) | Working C++ | ⬜ |
| 7 | [Deriving equality, hashing, and ordering from structure](posts/07-derive-eq-hash/) | Working C++ | ⬜ |

### Arc 3 — Serialization (`lib/reflect_json`)

| # | Post | Audience | Status |
|---|------|----------|--------|
| 8 | [A 40-line JSON serializer with reflection](posts/08-json-naive/) | Working C++ | ⬜ |
| 9 | [Annotations: tag-driven serialization in C++](posts/09-annotations/) | Working C++ | ⬜ |
| 10 | [Deserialization and `std::expected` error paths](posts/10-json-deserialize/) | Working C++ | ⬜ |
| 11 | [One codegen, many wire formats: YAML, TOML, MessagePack](posts/11-json-schema-yaml-toml/) | Working C++ | ⬜ |

### Arc 4 — Ambitious ports

| # | Post | Audience | Status |
|---|------|----------|--------|
| 12 | [CLI parsing: struct → typed command-line parser](posts/12-clap-for-cpp/) | Working C++ | ⬜ |
| 13 | [A tiny ORM: struct → SQL](posts/13-tiny-orm/) | Working C++ | ⬜ |
| 14 | [Autowired dependency injection](posts/14-dependency-injection/) | Working C++ | ⬜ |
| 15 | [Auto-generated mocks from interfaces](posts/15-auto-mocks/) | Working C++ | ⬜ |

### Arc 5 — Advanced / synthesis

| # | Post | Audience | Status |
|---|------|----------|--------|
| 16 | [`define_aggregate`: synthesizing types at compile time](posts/16-define-aggregate/) | Working C++ | ⬜ |
| 17 | [Replacing Qt's MOC with reflection](posts/17-qt-moc-replacement/) | Working C++ | ⬜ |
| 18 | [Capstone: C++26 reflection vs Rust, C#, Java, TS, Go, Python](posts/18-cross-language-comparison/) | Polyglot | ⬜ |

---

## How to follow

- Star the repo; each published post ships as a tagged release (`post-01`, `post-02`, ...).
- Every post links to a **GitHub Discussion** thread — that's where questions and corrections live.
- Issues are reserved for bugs in example code or the Docker toolchain.

## Caveats

- The Bloomberg fork is explicitly **not production-ready** per its own README. Use it to learn and experiment with P2996, not to ship.
- The pinned SHA will bump periodically as P2996 evolves. Old posts link to specific SHAs so historical examples always compile.
- Annotations (P3394) syntax may change; posts that depend on annotations name the exact SHA they were verified against.

## Local verification

`./verify.sh` walks every post's `examples/` directory, compiles + runs each sample, and prints pass/fail. Required before tagging a release.

## License

Code examples: MIT. Prose: CC BY 4.0.
