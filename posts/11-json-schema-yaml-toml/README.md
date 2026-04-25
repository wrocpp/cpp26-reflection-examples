# One codegen, many wire formats

> **Audience:** working C++ devs.
> **Reading time:** ~15 minutes.
> **Prerequisites:** posts [2](../02-first-reflection/) (reflecting members), [4](../04-expansion-statements/) (`template for`), [8](../08-json-naive/) (the naive JSON serializer), [9](../09-annotations/) (annotations).
> **Part 11** of the [C++26 Reflection series](../../README.md).

In the previous arc we grew `lib/reflect_serial` from a 40-line JSON demo into an annotated serializer handling rename, skip, skip-if-empty, and container-level naming policies. This post takes the same library and, with one structural change, makes it emit **JSON, YAML, XML, TOML, MessagePack, or anything else** that fits the "object = key-value pairs, possibly nested" shape. No new reflection machinery, no re-walking of members, no change at the call site. One struct, many formats.

Drop-in demo:

```cpp
User u{"filip", 42, "filip@example.com", "hash", true, {"Warsaw", 12345}};
std::println("{}", rserial::to_json(u));
std::println("{}", rserial::to_yaml(u));
std::println("{}", rserial::to_xml(u));
```

```
{"userName":"filip","id":42,"email":"filip@example.com","isAdmin":true,"homeAddress":{"city":"Warsaw","postal_code":12345}}

userName: filip
id: 42
email: filip@example.com
isAdmin: true
homeAddress: 
  city: Warsaw
  postal_code: 12345

<userName>filip</userName><id>42</id><email>filip@example.com</email><isAdmin>true</isAdmin><homeAddress><city>Warsaw</city><postal_code>12345</postal_code></homeAddress>
```

Same struct, same annotations (`[[=rename_all(camel_case)]]`, `[[=json_name("id")]]`, `[[=skip{}]]`), three wire formats. The full demo is in [`examples/formats_demo.cpp`](./examples/formats_demo.cpp).

## The observation

Look at what the naive `to_json` from post 8 was actually doing:

```cpp
out += '{';
template for (constexpr auto m : members) {
    if (!first) out += ',';
    out += '"'; out += key; out += "\":";
    emit_value(out, obj.[:m:]);
}
out += '}';
```

The `'{'`, `'}'`, `','`, `'"'`, and `':'` are format-specific. They're the *only* format-specific bits. The reflection walk itself — "enumerate members, resolve the key, recurse for nested types, stop on `skip` annotations" — is universal. If we pull out the syntax and leave the walk alone, the walk works for any format that fits this shape.

## The factoring

Identify what changes between formats. For JSON / YAML / XML / TOML / MessagePack, it's a small list:

- How an object opens and closes (`{}` vs. indentation vs. `<tag>` vs. nothing vs. length-prefix byte).
- How fields are separated (`,` vs. newline vs. nothing vs. vtable byte).
- How a key is rendered (`"k":` vs. `k: ` vs. `<k>` vs. `k = ` vs. varint-len).
- How a leaf value is rendered (quoted-escaped vs. bare vs. tagged vs. typed-binary).
- Whether there's a post-field hook (XML needs to close `</k>`).

That is seven methods. Collect them into a struct — a **format policy** — and pass it as the first template parameter to the serializer.

```cpp
struct json_format {
    void begin_object(std::string& o)    { o += '{'; }
    void end_object(std::string& o)      { o += '}'; }
    void field_separator(std::string& o) { o += ','; }
    void end_field(std::string&)         {}
    void emit_key(std::string& o, std::string_view k) {
        o += '"'; o += k; o += "\":";
    }
    void emit_string(std::string& o, std::string_view v) { o += '"'; o += v; o += '"'; }
    void emit_bool  (std::string& o, bool v)             { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)                { o += std::to_string(v); }
};
```

Seven methods. That's the contract. Any type that supplies them is a format.

## YAML — depth as format state

YAML wants indentation, which means the format needs **state**: the current depth. That's fine; format policies are ordinary objects.

```cpp
struct yaml_format {
    int depth = 0;
    void begin_object(std::string& o) {
        if (depth > 0) o += '\n';
        ++depth;
    }
    void end_object(std::string&)        { --depth; }
    void field_separator(std::string& o) { o += '\n'; }
    void end_field(std::string&)         {}
    void emit_key(std::string& o, std::string_view k) {
        for (int i = 0; i < depth - 1; ++i) o += "  ";
        o += k; o += ": ";
    }
    void emit_string(std::string& o, std::string_view v) { o += v; }
    void emit_bool  (std::string& o, bool v)             { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)                { o += std::to_string(v); }
};
```

Root object: depth starts at 0, no leading newline. Nested object: `begin_object` emits a newline before incrementing, so the nested block starts on a fresh line at the right indent.

## XML — tag stack as format state

XML has matched open/close tags, which means `end_field` can't just append a separator — it has to know the *name* of the tag it's closing. The `emit_key` / `end_field` pair forms a push / pop.

```cpp
struct xml_format {
    std::vector<std::string_view> tags;
    void begin_object(std::string&)      {}
    void end_object(std::string&)        {}
    void field_separator(std::string&)   {}
    void end_field(std::string& o) {
        o += "</"; o += tags.back(); o += '>';
        tags.pop_back();
    }
    void emit_key(std::string& o, std::string_view k) {
        tags.push_back(k);
        o += '<'; o += k; o += '>';
    }
    void emit_string(std::string& o, std::string_view v) { o += v; }
    void emit_bool  (std::string& o, bool v)             { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)                { o += std::to_string(v); }
};
```

Nesting works out because `emit_key` is always followed by either a leaf `emit_*` or a recursive `serialize_to` — and the recursive case pushes its own tags and pops them on the way back up. When we return to the outer `end_field`, the outer tag is at the back of the stack.

## The core engine

The engine is format-agnostic. It receives a policy by reference, delegates to it at every syntactic decision, and handles the **structure** (member walk, skip, nesting).

```cpp
template <typename F, typename T>
void serialize_to(F& fmt, std::string& out, T const& obj);

template <typename F, typename V>
void emit_value(F& fmt, std::string& out, V const& v) {
    if constexpr (std::is_same_v<V, bool>)                     fmt.emit_bool(out, v);
    else if constexpr (std::is_arithmetic_v<V>)                fmt.emit_number(out, v);
    else if constexpr (std::is_convertible_v<V, std::string_view>) fmt.emit_string(out, v);
    else                                                        serialize_to(fmt, out, v);
}

template <typename F, typename T>
void serialize_to(F& fmt, std::string& out, T const& obj) {
    fmt.begin_object(out);
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (!std::meta::annotation_of_type<skip>(m).has_value()) {
            if (!first) fmt.field_separator(out);
            first = false;
            fmt.emit_key(out, key_of<m>());
            emit_value(fmt, out, obj.[:m:]);
            fmt.end_field(out);
        }
    }
    fmt.end_object(out);
}
```

Two compile-time things deserve naming here:

- **`emit_value`'s dispatch is `if constexpr`.** Each instantiation picks exactly one branch. For a numeric field the `emit_string` call never exists in the generated code; for a nested struct the recursion target is resolved at compile time. No runtime dispatch, no function-pointer indirection.
- **`skip` is a compile-time pruning.** The annotation check is `if constexpr`, so fields marked `[[=skip{}]]` don't just get "not emitted at runtime" — they never appear in the compiled function at all.

The public API is a one-liner per format:

```cpp
template <typename T> std::string to_json(T const& obj) {
    json_format fmt; std::string out;
    serialize_to(fmt, out, obj);
    return out;
}
template <typename T> std::string to_yaml(T const& obj) { /* yaml_format */ }
template <typename T> std::string to_xml(T const& obj)  { /* xml_format */ }
```

## What gets generated

For a concrete `User` struct, each call instantiates its own straight-line function:

- `serialize_to<json_format, User>` compiles to something morally equivalent to
  ```cpp
  out += "{\"userName\":\""; out += obj.user_name;
  out += "\",\"id\":"; out += std::to_string(obj.user_id);
  /* ... */
  out += "}";
  ```
- `serialize_to<yaml_format, User>` compiles to the equivalent indented line-per-field form.
- `serialize_to<xml_format, User>` compiles to push/append/pop sequences.

Three separate functions. Zero shared metadata, no runtime vtable, no reflection cost at runtime. The annotation keys (`"userName"`, `"id"`, `"email"`) land in `.rodata` as `define_static_string` literals. The `skip` check is dead code the optimizer never sees because it was pruned during template instantiation.

## Adding a fourth format: TOML

What does TOML look like? Flat key-value lines at the top level:

```toml
userName = "filip"
id = 42
email = "filip@example.com"
isAdmin = true
```

Nested structs are `[table]` sections:

```toml
[homeAddress]
city = "Warsaw"
postal_code = 12345
```

A minimal `toml_format`:

```cpp
struct toml_format {
    std::string section_path;

    void begin_object(std::string& o) {
        if (!section_path.empty()) {
            o += "["; o += section_path; o += "]\n";
        }
    }
    void end_object(std::string&) {}
    void field_separator(std::string& o) { o += '\n'; }
    void end_field(std::string&) {}

    void emit_key(std::string& o, std::string_view k) {
        o += k; o += " = ";
    }
    void emit_string(std::string& o, std::string_view v) {
        o += '"'; o += v; o += '"';
    }
    void emit_bool  (std::string& o, bool v) { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)    { o += std::to_string(v); }
};
```

(Nested tables need the engine to tell the format "we're starting section X" — in a real implementation you'd extend the policy contract with a `begin_section(name)` hook, or pass the section path through a small adapter. The shape of the extension is straightforward; the point is that the reflection walk doesn't change.)

## Adding a binary format: MessagePack

MessagePack is more interesting because the policy emits bytes, not characters:

```cpp
struct msgpack_format {
    void begin_object(std::vector<std::uint8_t>& o, std::size_t field_count) {
        // fixmap: 1000XXXX for up to 15 fields
        o.push_back(0x80 | static_cast<std::uint8_t>(field_count));
    }
    void emit_key(std::vector<std::uint8_t>& o, std::string_view k) {
        o.push_back(0xa0 | static_cast<std::uint8_t>(k.size())); // fixstr
        o.insert(o.end(), k.begin(), k.end());
    }
    /* ... bool, number, string emitters writing MsgPack bytes ... */
};
```

This doesn't fit the textual-policy contract exactly — the output buffer type changes (`std::vector<std::uint8_t>` instead of `std::string`), and `begin_object` needs the *count* of upcoming fields (MessagePack prefixes containers with their size). Both extensions are mechanical: the engine's `std::string&` becomes a template parameter `Buffer&`, and the engine counts the surviving (non-skipped) fields at compile time with a small `consteval` helper before calling `begin_object`.

Same walk. Different output type. Same 100-line engine.

## Per format, what it buys you

| Format | State carried | Unique pattern | Output type |
|---|---|---|---|
| JSON | — | braces + commas + quoted keys | `std::string` |
| YAML | `int depth` | newline + per-depth indent | `std::string` |
| XML | `vector<string_view>` tag stack | open-tag / close-tag pairing via `end_field` | `std::string` |
| TOML | section path | flat keys + `[section]` headers for nesting | `std::string` |
| MessagePack | — (or field count) | length-prefixed keys/values, byte output | `std::vector<uint8_t>` |
| Protobuf wire format | field-number map | tag/length/value, requires `[[=field_number(N)]]` annotation | `std::vector<uint8_t>` |

The columns that change are small. The reflection walk is the same. The annotation vocabulary (`rename`, `skip`, `skip_if_empty`, `rename_all`) is the same and applies uniformly.

## Cross-language angle

| Language | "One model, many formats" story |
|---|---|
| **Rust serde** | Separate crates (`serde_json`, `serde_yaml`, `serde_toml`, `rmp_serde`). Each implements the `Serializer` trait. The `#[derive(Serialize)]` on your struct calls whichever one is linked. Conceptually the same factoring — polymorphic over a trait. |
| **Jackson (Java)** | `ObjectMapper`, `YAMLMapper`, `XmlMapper`, `CBORMapper`, etc. Same annotated classes; swap the mapper. Runtime polymorphism via method dispatch. |
| **C# System.Text.Json** | JSON only. YAML/XML go through separate libraries with their own attribute dialects. No unified model. |
| **Go encoding/...** | `encoding/json`, `encoding/xml`, third-party `yaml.v3`. Each reads struct tags independently. No unified model; tags must be duplicated per format (`json:"..."` + `yaml:"..."` + `xml:"..."`). |
| **Pydantic** | `model_dump_json()`, `model_dump()` (dict), third-party for YAML. |
| **C++26** | One policy struct per format. The reflection walk is the shared core; annotations apply across formats. No external library per format — just the engine and N small policy files. |

The C++ angle: both serde's trait-based polymorphism (same model, different serializer) and the Rust/Jackson ergonomics (one annotation vocabulary works everywhere) end up in the same place — with the entire dispatch resolved at compile time. The output is the codegen of a hand-written serializer for each format.

## Try it

```sh
./cpp posts/11-json-schema-yaml-toml/examples/formats_demo.cpp \
      -o posts/11-json-schema-yaml-toml/examples/formats_demo
./run posts/11-json-schema-yaml-toml/examples/formats_demo
```

Compiler Explorer: [permalink TBD](./examples/godbolt.md).

**Exercise:** write a `csv_format` policy. Hint: `begin_object` on the root emits the header row, `begin_object` on nested objects is a no-op, `emit_key` discards the key (or builds the header), `emit_string` quotes when the value contains commas or quotes. It fits the 7-method contract with one subtlety — CSV has no nested structure, so either your model must be flat or the policy flattens on its own.

## What's next

- **[Post 12 — CLI parsing: struct → typed command-line parser](../12-clap-for-cpp/)** starts Arc 4. We leave serialization and port Rust's clap-derive to C++26.
- **[Post 18 — Reflection across languages](../18-cross-language-comparison/)**: the capstone. The canonical data tasks side-by-side across Rust / Java / C# / TypeScript / Go / Python / C++26.

Questions, corrections, your own wire-format policy: [Discussions](TODO-discussion-link).
