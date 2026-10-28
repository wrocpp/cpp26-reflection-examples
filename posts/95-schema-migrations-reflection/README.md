# Schema migrations as lambdas: what C++26 reflection changes

Companion code for the wro.cpp flagship replying to Leonardo Rossi's LinkedIn post on writing schema
migrations as lambdas in an array, with the migration's target type read from the lambda's parameter.

Leonardo reads the target with a `member_traits` trait specialized once per cv / ref / noexcept
combination of `operator()`. This example asks what C++26 reflection changes about that price.

## Verified on GCC 16.2.0 (`-std=c++26 -freflection`, `-Wall -Wextra -Werror`, run at -O0 and -O2)

- **One reflection query replaces the whole trait family.** `members_of` surfaces exactly the lambda's
  `operator()` (`is_function && is_operator_function`, not the conversion-to-pointer operator, not a
  special member); `type_of(parameters_of(op)[0])` with `remove_cvref_t` yields the target in one go.
  Proven with a single `TargetOf` across four shapes: `Database&`, `const Database&`, `Database&&`,
  `Database& noexcept` all deduce `Database`.
- **The deduction guide fires** on the reflection `TargetOf`: `Migration<Database>` is deduced from the
  lambda alone.
- **Generic lambdas stay out, even with reflection** -- the generic lambda's only member is
  `template<class auto:N> <lambda(auto:N&)>` with `is_function == false`, `is_function_template == true`.
  There is no concrete function and no single parameter to read, so the query finds nothing. This is
  the same conclusion Leonardo reached by hand.
- **Deduction and storage are separate.** `TargetOf` strips cv/ref to pick the class, but a stored
  `void(*)(Database&)` only accepts a `Database&` lambda; a `const Database&` lambda deduces the same
  `Migration<Database>` yet will not convert to that fixed pointer signature.

Files: `examples/target_of_reflection.cpp`.
