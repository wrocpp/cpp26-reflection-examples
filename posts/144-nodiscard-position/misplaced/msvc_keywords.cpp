// verify: gcc-only
// [[nodiscard]] after two Microsoft keywords. MSVC only: GCC and Clang do not accept these keywords
// without extensions.
//
// Expected: MSVC at /W4 compiles the file with no diagnostic; /we5240 turns each line into error C5240.
constexpr int answer_value = 42;

__forceinline [[nodiscard]] int forced_answer() { return answer_value; }
__declspec(noinline) [[nodiscard]] int outlined_answer() { return answer_value; }

int main() {
    forced_answer();
    outlined_answer();
    return 0;
}
