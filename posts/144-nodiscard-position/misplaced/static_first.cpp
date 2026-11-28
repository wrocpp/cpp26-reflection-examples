// verify: gcc-only
// The misplaced form from Giovanni Dicanio's post: static, then [[nodiscard]].
//
// Expected: GCC and Clang reject the declaration. MSVC at /W4 compiles it with no diagnostic,
// the attribute is dropped, and the discarded call in main() is not reported.
//   cl /std:c++20 /W4 static_first.cpp          -> no diagnostic
//   cl /std:c++20 /W4 /we5240 static_first.cpp  -> error C5240
// The program returns 0 when it runs.
static [[nodiscard]] int get_something() { return 0; }

int main() {
    get_something();
    return 0;
}
