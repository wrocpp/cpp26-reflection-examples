# A two-pass assembler at compile time

Companion to the wro.cpp post on labels, inspired by Andrew Alekseev's LinkedIn post about an assembler
bootstrapped from find-and-replace (the project his post links to is github.com/blankyet/BlankedASM, which
has no license, so none of its code is used here).

`examples/constexpr_asm.cpp` assembles nine real x86-64 instructions in a `constexpr` function with two
passes (labels first, bytes second), checks the 44 bytes with a `static_assert` against a table written
by hand from the encodings, and, on x86-64 Linux, copies them into executable memory and runs them.
Compiler Explorer (g162): https://godbolt.org/z/M3TcGq99K

## Checked

- The bytes equal the hand-written table (`static_assert`, GCC 16.2, `-Wall -Wextra -Werror`, -O0 and -O2).
- `llvm-mc --disassemble` (LLVM 23.1.1) reads them back as the source program, with `jne -23` and `jmp 1`.
- Run natively on Compiler Explorer (x86-64 g162): `rax = 6`, at -O0 and -O2.
- An undefined label is a compile error: `call to non-'constexpr' function 'void casm::fail(const char*)'`.

## Not covered

Only `rax` and `rcx`, nine instruction forms, 32-bit displacements for every jump (no short forms), no
data directives, no error messages beyond which line failed. It is a model of the label problem, not an assembler.
