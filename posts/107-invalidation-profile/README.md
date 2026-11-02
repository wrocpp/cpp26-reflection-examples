# The invalidation profile against P4418R0 and real code

Companion to the wro.cpp post `invalidation-profile-against-real-code`. The code runs on the
experimental Compiler Explorer build `gcontracts-p4324-trunk` ("x86-64 gcc (P4324 contracts, and
profiles)"), a GCC branch with a first invalidation profile. It is not a shipping compiler.

- `examples/invalidation_cases.cpp`: the forms the profile accepts, which the default build compiles
  and runs, and one macro per case the profile rejects or that I expected it to reject.
  Compiler Explorer (`gcontracts-p4324-trunk`, `-std=c++23 -pedantic-errors`): https://godbolt.org/z/h8KW8xY6T

Add `-DSHOW_<NAME>` to the options to compile one case:

| macro | source of the case | result on 2 November 2026 |
|---|---|---|
| `SHOW_PAPER_SECTION_1` | P4418R0 section 1 | 3 errors and a GCC warning |
| `SHOW_DELETE_THEN_READ` | section 1, with `[[owner]]` | 1 error |
| `SHOW_CONDITIONAL_DELETE` | section 2, with `[[owner]]` | 5 errors |
| `SHOW_OWNER_THROWN_AWAY` | section 8 | 2 errors |
| `SHOW_NOT_INVALIDATING_ON_FUNCTION` | section 3 attribute on a free function | 1 error (not a member or parameter) |
| `SHOW_NOT_INVALIDATING_NOT_VERIFIED` | section 3 says the attribute is verified | accepted, no message |
| `SHOW_NOT_INVALIDATING_MEMBER` | same, on a member function | accepted, no message |
| `SHOW_RETURN_LOCAL_ADDRESS` | section 1 `return &x;` | 1 error and a GCC warning |
| `SHOW_STORE_LOCAL_IN_GLOBAL` | section 6 `glob_ptr = &x;` | accepted, no message |
| `SHOW_VECTOR_OF_POINTERS` | section 6 | 1 error, for the local one only |
| `SHOW_LAMBDA_RETURNED` | section 4 lambda capturing by reference | 1 error |
| `SHOW_LAMBDA_STORED` | same lambda stored in a global `std::function` | accepted, no message |
| `SHOW_THIS_STORED` | `[this]` stored in a member and returned | accepted, no message |
| `SHOW_THIS_DETACHED` | `[this]` handed to a detached thread | accepted, no message |
| `SHOW_THREAD_DETACHED_REFERENCE` | reference capture in a detached thread | accepted, no message |
| `SHOW_JTHREAD_RETURNED` | section 4 `jthread` holding a pointer | 1 error |
| `SHOW_LOCAL_LOGGER` | section 7, factory over a local `Logger` | 1 error |
| `SHOW_LOCAL_LOGGER_NO_DANGLING` | same, `std::no_dangling` around the call | accepted |
| `SHOW_FACTORY_UNMARKED` | section 7 factory without `std::no_dangling` inside | 1 error, inside the factory |
| `SHOW_NO_ATTRIBUTE_CONTROL` | control for the attribute on a parameter | 1 error |
| `SHOW_MEMBER_NO_ATTRIBUTE_CONTROL` | control for the attribute on a member | 1 error |
| `SHOW_NO_DANGLING_UNQUALIFIED` | section 7 spells it `no_dangling(...)` | 1 error, not declared |

The three Clang builds (`clang_profiles_init`, `clang_profiles_core_ub`, `clang_cppa_p4324`) compile
every case with exit 0 and no error, apart from the unqualified `no_dangling` case. They print
unknown-attribute warnings and Clang's own `-Wreturn-stack-address`. Clang has no `std::no_dangling`,
so for those runs each `std::no_dangling(x)` was replaced by `x`.

Both builds follow development branches, so the output can change.
