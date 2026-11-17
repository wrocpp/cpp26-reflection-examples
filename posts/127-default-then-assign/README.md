# default-then-assign: objects built in one braced expression

Companion to the wro.cpp short about the `wrocpp-default-then-assign` check of tidy-agent. The examples contain no macros.

- `examples/before.cpp`: five functions that build an object in steps. Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/P5x8MT6n8
- `examples/after.cpp`: the same five functions with each value in one braced expression. Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/qd4xGG7MP
- `examples/double_designator.cpp`: a designated initializer that names one member twice. It is not meant to compile with GCC; see below.

Program results were taken on Compiler Explorer on 2026-11-17. The check ran locally on 2026-11-17.

## Program output

`before.cpp` and `after.cpp` print the same five lines and exit 0 (g162 and clang2210, both `-std=c++26 -O1 -Wall -Wextra`, no warnings):

```text
request_for: /orders timeout=30 retry=1
request_with_overwrite: /orders timeout=5 retry=0
delays: 100 200 400
headers: 3
doubled: 200
```

`request_with_overwrite` assigns `timeout_seconds` twice (30, then 5). Nothing reports it.

## The check

`wrocpp-default-then-assign` is a plugin check of https://github.com/wrocpp/tidy-agent, run locally. It is not a query check, so Compiler Explorer cannot run it.

- LLVM and clang-tidy 23.1.5 (Homebrew), plugin built from the repository at commit 7c64b6c against the same LLVM.
- Command: `clang-tidy -load=libwrocpp-tidy.so --checks='-*,wrocpp-default-then-assign' before.cpp -- -std=c++26`

```text
before.cpp:40:11: warning: 'request' is default-constructed and then assigned field by field; initialize it in one braced expression [wrocpp-default-then-assign]
   40 |   Request request;
      |           ^
      |                  {.path = std::move(path), .timeout_seconds = kDefaultTimeoutSeconds, .retry = true}
before.cpp:48:11: warning: 'request' is default-constructed and then assigned field by field; initialize it in one braced expression [wrocpp-default-then-assign]
before.cpp:57:20: warning: 'delays' is default-constructed and then filled by 3 mutator calls; initialize it with a braced list [wrocpp-default-then-assign]
before.cpp:65:11: warning: 'headers' is default-constructed and then filled by 3 mutator calls; initialize it with a braced list [wrocpp-default-then-assign]
```

Four warnings. `doubled` (a loop over runtime data) is not flagged. Only the first warning carries a fix-it. The second has none
because the same member is assigned twice, and the container and the class with a constructor never get one.

## --fix

With a `.clang-format` of `BasedOnStyle: Google` and `ColumnLimit: 120`, the command

```text
clang-tidy -load=libwrocpp-tidy.so --checks='-*,wrocpp-default-then-assign' --fix --format-style=file before.cpp -- -std=c++26
```

reports `clang-tidy applied 4 of 4 suggested fixes.` and changes `before.cpp` like this:

```diff
 Request request_for(std::string path) {
-  Request request;
-  request.path = std::move(path);
-  request.timeout_seconds = kDefaultTimeoutSeconds;
-  request.retry = true;
+  Request request{.path = std::move(path), .timeout_seconds = kDefaultTimeoutSeconds, .retry = true};
   return request;
 }
```

Nothing else in the file changes. The fixed file compiled with g++ 16 and printed the five lines above. Without
`--format-style=file`, the line after the new initializer kept the indentation of a removed line (`     return request;`).

## A member named twice

`double_designator.cpp` names `timeout_seconds` twice in one designated initializer (g162, `-std=c++26 -O1`, build fails):

```text
<source>:24:30: error: '.timeout_seconds' designator used multiple times in the same initializer list
<source>:25:25: error: could not convert '{"/orders", ...}' from '<brace-enclosed initializer list>' to 'Request'
```

clang2210 (`-std=c++26 -O1`) compiles the same file and exits 0. It prints
`warning: initializer overrides prior initialization of this subobject [-Winitializer-overrides]`, so on Clang 22.1 the second
value is not a compile error.
