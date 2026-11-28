// verify: gcc-only
// [[nodiscard]] after one keyword of the declaration, for six standard keywords. Every call below
// discards its result.
//
// Expected: GCC 16.2 rejects five of the six ("standard attributes in middle of decl-specifiers")
// and only warns "attribute ignored" for the explicit constructor. Clang rejects all six.
// MSVC at /W4 compiles the file with no diagnostic; /we5240 turns each line into error C5240.
constexpr int answer_value = 42;

static [[nodiscard]] int static_answer() { return answer_value; }
inline [[nodiscard]] int inline_answer() { return answer_value; }
constexpr [[nodiscard]] int constexpr_answer() { return answer_value; }

struct Base {
    virtual [[nodiscard]] int answer() const { return answer_value; }
    virtual ~Base() = default;
};

struct Token {
    friend [[nodiscard]] int answer(Token) { return answer_value; }
};

struct Guard {
    explicit [[nodiscard]] Guard(int) {}
};

int main() {
    const Base base{};
    static_answer();
    inline_answer();
    constexpr_answer();
    base.answer();
    answer(Token{});
    Guard{answer_value};
    return 0;
}
