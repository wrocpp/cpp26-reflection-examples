# C++26 Reflection: What changes, and why it matters

> **Audience:** polyglot — if you write C++, you'll recognise the pain; if you come from Rust, Java, C#, TypeScript, Go, or Python, you'll recognise what C++ has been missing.
> **Reading time:** ~12 minutes.
> **Part 1** of the [C++26 Reflection series](../../README.md).

Here is a C++ program that serializes any struct to JSON:

```cpp
struct Address {
    std::string city;
    int postal_code;
};

struct User {
    std::string name;
    int age;
    bool admin;
    Address home;
};

int main() {
    User u{"Filip", 40, true, {"Warsaw", 12345}};
    std::println("{}", rjson::to_json(u));
    // -> {"name":"Filip","age":40,"admin":true,"home":{"city":"Warsaw","postal_code":12345}}
}
```

The serializer lives in roughly forty lines of library code. The `User` struct has no JSON-specific macros, no inheritance from a base class, no listed-again field names, no external `.proto` file, no code generator you need to rerun when you add a field. Add a new field; it appears in the JSON output. Rename a field; the JSON key follows. Nested struct; handled.

If you come from Rust, Java, C#, TypeScript, Go, or Python, you are wondering why this is news. Your language has had the equivalent for a decade or more, often in a single line:

```rust
#[derive(Serialize)] struct User { name: String, age: u32, admin: bool }
```

For C++, code like this was essentially impossible before 2026. The whole job — "take a struct, figure out its field names at compile time, and do something per-field" — required a preprocessor, an external tool, or several hundred lines of template metaprogramming. Often all three.

That changed with C++26's [static reflection (P2996)](https://wg21.link/P2996), plus a handful of companion proposals. This post explains why a feature that looks mundane in isolation is one of the most significant additions to C++ in a generation.

## Act 1 — The tax we've been paying

Before reflection, getting any form of introspection in C++ meant one of three strategies, each with its own cost.

**Preprocessor macros.** `nlohmann/json`, the most popular C++ JSON library, asks you to write:

```cpp
struct User { std::string name; int age; bool admin; };
NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, name, age, admin)
```

It works. It also means every field name appears three times — in the struct, in the macro, and in the JSON key. Rename a field in the struct and forget the macro and you get a compile error if you're lucky, a silent schema-drift bug if you're not. Across a 200-field data model this gets old fast. The same pattern — "declare your type, then declare your type again to a macro" — runs through Boost.Describe, `BOOST_HANA_DEFINE_STRUCT`, cereal's `serialize()` archive functions, and most reflection-adjacent C++ libraries in existence.

**External code generators.** Qt's [MOC](https://doc.qt.io/qt-6/moc.html) is effectively a second compiler that parses `Q_OBJECT` and `Q_PROPERTY` and emits glue code. It ships with every Qt install. It confuses every IDE that doesn't know about it. Protocol Buffers ships `protoc`. gRPC ships its own plugin on top of `protoc`. ODB ships its own ORM code generator. Thrift, Cap'n Proto, FlatBuffers — all bring their own compilers. Each one is a tool you must install, version, integrate with CMake or Bazel or Meson, and keep in lockstep with the rest of your toolchain. Every one of them is a supply-chain liability and a new category of build-system bug.

**Compiler hacks.** [`magic_enum`](https://github.com/Neargye/magic_enum) gives you `enum <-> string`, but only by extracting names from `__PRETTY_FUNCTION__` — a compiler-specific string — in a loop over a fixed range (default -128..128). It's clever and it's useful, and it's also the kind of code that has compile times measured in seconds per header, ships a `MAGIC_ENUM_RANGE_MIN`/`MAGIC_ENUM_RANGE_MAX` knob because one size doesn't fit all, and breaks the day a compiler decides to format its diagnostic strings differently.

None of these are bad libraries. They are the best available answers to "how do I walk over a C++ struct's fields" in a language that didn't let you walk over a struct's fields. But they all come with a tax: extra build steps, duplicated names, special-case error messages, IDE confusion, slower compile times, and learning curves whose first mile is "figure out what the macros are doing."

## Act 2 — The gap we've been living with

Compare C++'s "pay the tax" patterns to what sits in the default tool belt of most modern languages.

| Language | Serialize a struct to JSON |
|---|---|
| **Rust** | `#[derive(Serialize)] struct User { name: String, age: u32 }` |
| **C#** | `public record User(string Name, int Age);` — `System.Text.Json` handles it. |
| **Java** | `public record User(String name, int age) {}` — Jackson via runtime reflection. |
| **TypeScript** | `JSON.stringify({ name, age })` — object properties are introspectable by construction. |
| **Go** | `type User struct { Name string \`json:"name"\`; Age int \`json:"age"\` }` |
| **Python** | `@dataclass class User: name: str; age: int` — `json.dumps(dataclasses.asdict(u))`. |
| **C++ (before)** | A 30-line `NLOHMANN_DEFINE_TYPE_INTRUSIVE` + helper block, or an ODB model file, or a Boost.Describe incantation. |

The gap isn't about performance. C++ has always been fast. It's about **ergonomics and ecosystem leverage**: the ability to write a library that says "give me a struct and I'll handle the boilerplate" without also demanding that users install a code generator, duplicate every field name, or learn a template-metaprogramming dialect.

C++ has been trying to close this gap for almost twenty years. [N1958 (2006)](https://open-std.org/JTC1/SC22/WG21/docs/papers/2006/n1958.pdf) proposed reflection. N3956 (2014), N4428 (2015), P0194 (2016), the Reflection TS (2019), and P1240 "Scalable Reflection" (2020) all came and went. The committee couldn't agree on API shape: reflections as types or as values? runtime or compile-time? how do they compose with concepts and templates? should there be a separate meta-language? Each proposal had merit; each stalled on a different detail.

After seven major proposals and most of two decades, [P2996 "Reflection for C++26"](https://wg21.link/P2996) (Daveed Vandevoorde, Dan Katz, Barry Revzin, Faisal Vali) converged — value-based, compile-time, splicer-driven, and implementable. Adopted into C++26. Shipping in compilers now.

## Act 3 — The answer

Here is the `to_json` function we skipped at the top, in its entirety:

```cpp
template <typename T>
std::string to_json(T const& obj) {
    std::string out = "{";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto member
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ',';
        first = false;
        append_quoted(out, std::meta::identifier_of(member));
        out += ':';
        append_value(out, obj.[:member:]);
    }
    out += '}';
    return out;
}
```

Five new things are happening:

1. **`^^T`** is the *reflect operator*. It takes an entity — here, a type — and yields a value of type `std::meta::info`, an opaque compile-time handle. You can reflect types, variables, functions, namespaces, templates, expressions, and more.

2. **`std::meta::nonstatic_data_members_of(^^T, ctx)`** asks the compiler for the list of `T`'s fields as a `std::vector<std::meta::info>`. This evaluates at compile time; the vector is `consteval`.

3. **`std::define_static_array(...)`** takes that transient vector and materialises it into a compile-time `std::span` living in constant memory. (Required because a `constexpr` variable cannot hold a heap-allocated `vector`.)

4. **`template for`** is an *expansion statement* (proposal P1306): the compiler unrolls the loop at compile time, instantiating the body once per member. Each iteration gets its own `constexpr auto member`.

5. **`[:member:]`** is the *splicer*. It turns a reflection back into code. `obj.[:member:]` becomes, per iteration, `obj.name`, then `obj.age`, then `obj.admin`, then `obj.home`. The compiler writes the member accesses for you at compile time.

Look at what this code *isn't*:

- It isn't **runtime reflection** (Java, C#, Python). There is no lookup table at runtime, no RTTI, no heap allocation in the compiled `to_json`. After compilation, the generated code is as fast as a hand-written specialisation.

- It isn't a **macro**. It's plain C++ syntax with first-class compiler support. Your IDE navigates into it. Error messages point at real source locations. Your debugger steps through it.

- It isn't an **external code generator**. No separate build step. No `protoc` version to pin. No generated files to gitignore. No tooling-out-of-sync problems.

- It isn't a **template metaprogramming trick**. No recursive instantiation, no SFINAE walls, no `if constexpr` ladder that only works for aggregates of arity 1 through 32.

### Sidebar: hoisting the "first-member" branch out to compile time

Readers with C++ intuition will have flinched at `bool first = true; if (!first) ...` inside `template for`. The classical "peel the first iteration" idiom does better:

```cpp
auto it = c.begin();
if (it != c.end()) out += *it;
for (++it; it != c.end(); ++it) { out += ","; out += *it; }
```

The same reshape works at compile time. `std::define_static_array` returns a `std::span<const std::meta::info>` — a literal type — so `members.empty()`, `members[0]`, and `members.subspan(1)` are all constant expressions:

```cpp
template <std::meta::info Member, typename T>
void append_member(std::string& out, T const& obj) {
    append_quoted(out, std::meta::identifier_of(Member));
    out += ':';
    append_value(out, obj.[:Member:]);
}

template <typename T>
std::string to_json(T const& obj) {
    std::string out = "{";
    if constexpr (constexpr auto members = std::define_static_array(
                      std::meta::nonstatic_data_members_of(
                          ^^T, std::meta::access_context::unchecked()));
                  !members.empty()) {
        append_member<members[0]>(out, obj);          // peeled first
        template for (constexpr auto m : members.subspan(1)) {
            out += ',';
            append_member<m>(out, obj);               // tail
        }
    }
    out += '}';
    return out;
}
```

Three things make this work, and it's worth naming each:

- **`if constexpr` with an init-statement.** `if (init; cond)` has been in C++ since C++17; combining it with `if constexpr` lets the reflection live exactly in the scope where it's used. `members` doesn't leak into the enclosing function body, and we dodge the "declare it, then immediately check it" two-line dance.
- **`members[0]` as a template argument.** The splicer `[:Member:]` needs `Member` to be a constant expression. Non-type template parameters are the cleanest way to thread a reflection into a shared helper — `append_member<members[0]>` is a valid call because `members` is `constexpr`.
- **`members.subspan(1)` as a `template for` range.** Expansion statements accept any constant-expression range; `span::subspan` is `constexpr`, so the tail becomes its own compile-time loop.

After `-O2`, the codegen is effectively identical — the optimiser proves the `first` flag's value per unrolled iteration and elides the branches either way. The win isn't runtime cost; it's source-level intent. The peel version has zero branches in the source, handles empty structs explicitly via `if constexpr`, and reads like "first, then rest" instead of "carry a flag".

Both styles are in the repo: [`examples/teaser.cpp`](./examples/teaser.cpp) (original) and [`examples/teaser_peel.cpp`](./examples/teaser_peel.cpp) (this one). Pick the one your team will read first.

---

Among the mainstream languages with large ecosystems — Rust, Java, C#, Go, Python, TypeScript — C++26 reflection is unique in combining **compile-time**, **zero runtime cost**, **type-safe**, and **not a preprocessor / codegen phase**. Java, C#, and Python pay for reflection at runtime: the information exists as data structures walked by the runtime. Rust's `#[derive(...)]` runs at compile time but via `proc_macro`, a separate code-generation layer that operates on tokens — it emits derived code but cannot inspect types beyond the macro's input. Go's `reflect` is runtime. TypeScript's introspection is either runtime (`Object.keys`) or type-system-only (`keyof`, `Pick`, `Omit`). Each of those is a legitimate choice. None of them gives you what P2996 gives C++: the full machinery of introspection, `consteval`, and constant evaluation fused together and available at the type system's front door.

(Languages like D and Zig have had compile-time reflection for a while; the point isn't that C++ is first, but that among the languages C++ shares a developer pool with, it finally catches up — and in doing so gets a design with the benefit of watching everyone else for twenty years.)

And because the reflection walk is just code — not a sealed macro — it composes with everything else the language offers. Annotations (`[[=rserial::json_name("user_name")]]`, `[[=rserial::skip{}]]`, `[[=rserial::rename_all(camel_case)]]`) port the serde / Jackson attribute vocabulary to C++ as plain values queryable at compile time ([`examples/teaser_annotated.cpp`](./examples/teaser_annotated.cpp)). The format is a pluggable policy: the *same* annotated struct compiles down to JSON, YAML, XML, or anything else you can describe — one policy struct per format, zero shared runtime metadata ([`examples/teaser_formats.cpp`](./examples/teaser_formats.cpp)). [Post 9](../09-annotations/) explores the annotation design in depth; [post 11](../11-json-schema-yaml-toml/) unpacks the format-policy factoring.

## Why this matters for the C++ community

Reflection isn't just a feature. It's an inflection point:

- **Libraries that previously shipped their own code generators become header-only.** A new generation of serialisation, ORM, RPC, CLI-parsing, and configuration libraries will look like `#include <rjson>` with no external tooling. Qt's MOC, protobuf's `protoc`, and ODB's compiler become historically interesting rather than required.

- **Macro incantations that plague C++ onboarding go away.** First-contact pain — `BOOST_PP_REPEAT`, `NLOHMANN_DEFINE_TYPE_INTRUSIVE`, `Q_OBJECT`, `MAGIC_ENUM_RANGE_MIN` — is replaced by plain C++ syntax that IDEs and debuggers understand.

- **The bar for writing a data-driven library drops dramatically.** Building "a serialiser like serde" becomes a weekend project instead of a year-long TMP odyssey. Library innovation has been bottlenecked by learning curve; P2996 removes that bottleneck.

- **C++ closes one of its most-cited gaps with modern languages.** "You can't even walk over a struct's fields" stops landing.

None of this is automatic. P2996 is new. The implementations are experimental — the [Bloomberg/clang-p2996 fork](https://github.com/bloomberg/clang-p2996) this series uses explicitly warns against production use. The library ecosystem will take years to catch up. Early adopters will hit compiler bugs, API shifts, and patterns that turn out to be wrong. But the shape of what's possible has changed, and it won't change back.

## Try it

The repository this post lives in has a pinned clang-p2996 Docker image and wrapper scripts. From a checkout:

```sh
./build.sh                                                         # one-time, ~30-60 min on arm64
./cpp posts/01-why-it-matters/examples/teaser.cpp \
      -o posts/01-why-it-matters/examples/teaser
./run posts/01-why-it-matters/examples/teaser
# -> {"name":"Filip","age":40,"admin":true,"home":{"city":"Warsaw","postal_code":12345}}
```

The peel-first variant from the sidebar is in the same folder: replace `teaser` with `teaser_peel` in both commands.

If you'd rather skip the install, Compiler Explorer hosts the clang-p2996 fork: [run the teaser on godbolt.org](./examples/godbolt.md).

## What's next

- **[Post 2 — Your first `^^`: reflecting types, walking members](../02-first-reflection/)** — the hands-on foundations. We take the teaser apart and rebuild it from primitives.
- **[Post 8 — A 40-line JSON serializer](../08-json-naive/)** — we revisit the teaser, scale it properly, and turn it into a real `lib/reflect_json`.
- **[Post 18 — Reflection across languages](../18-cross-language-comparison/)** — side-by-side with Rust, C#, Java, TypeScript, Go, and Python. The capstone.

Questions, corrections, or your own reflection war stories: drop them in [Discussions](TODO-discussion-link).
