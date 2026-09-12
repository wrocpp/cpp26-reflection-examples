// contract_decides.cpp
//
// Giovanni Dicanio asked an AI to review WinReg, his C++ wrapper over the
// Windows registry. It reported that a zero-length REG_SZ value would crash
// RegKey::GetStringValue and three siblings, and it had a real observation
// behind that: the binary getters guard against a zero byte count and the
// string getters do not.
//
// It was not a bug. The string getters call RegGetValueW, which by contract
// writes a NUL terminator even when the value stored in the registry has
// none, so the byte count on that path is never zero. The older
// RegQueryValueExW makes no such promise. Same reader code, two different
// answers, and nothing in the reader tells you which one you have.
//
// This file models the two contracts so the difference is visible without a
// Windows API. Nothing here reads out of bounds: the unsafe case is computed
// and reported rather than performed.
//
// Plain portable C++: no Windows headers, same output on GCC and clang.
//
// Compile: g++ -std=c++23 contract_decides.cpp

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

// ---------------------------------------------------------------------------
// Contract A, the RegQueryValueExW shape: copies exactly what is stored and
// reports exactly that many bytes. An empty stored value yields zero, and a
// non-empty one may or may not carry a terminator.
// ---------------------------------------------------------------------------
std::size_t query_raw(const char* stored, char* out, std::size_t out_cap)
{
    const std::size_t n = std::strlen(stored);
    if (n > out_cap) return 0;
    std::memcpy(out, stored, n);
    return n;                       // no terminator, no minimum
}

// ---------------------------------------------------------------------------
// Contract B, the RegGetValueW shape: guarantees the result is terminated,
// and counts the terminator in the size it reports. An empty stored value
// still costs one byte, so the count is never zero.
// ---------------------------------------------------------------------------
std::size_t get_value(const char* stored, char* out, std::size_t out_cap)
{
    const std::size_t n = std::strlen(stored);
    if (n + 1 > out_cap) return 0;
    std::memcpy(out, stored, n);
    out[n] = '\0';
    return n + 1;                   // terminator included
}

// ---------------------------------------------------------------------------
// The reader the AI flagged: take the reported size, subtract the terminator,
// build a string. No zero guard.
// ---------------------------------------------------------------------------
std::string read_unguarded(std::size_t size_in_bytes, const char* buf)
{
    const std::size_t chars = size_in_bytes - 1;   // drop the terminator
    return std::string(buf, chars);
}

int main()
{
    char buf[64];

    // The case the review was about: a value that is stored empty.
    const char* stored = "";

    const std::size_t a = query_raw(stored, buf, sizeof buf);
    const std::size_t b = get_value(stored, buf, sizeof buf);

    std::printf("empty stored value\n");
    std::printf("  contract A (query_raw) reports : %zu bytes\n", a);
    std::printf("  contract B (get_value) reports : %zu bytes\n", b);

    // Under contract B the unguarded reader is correct: 1 - 1 == 0 characters.
    const std::size_t b_buf = get_value(stored, buf, sizeof buf);
    const std::string s = read_unguarded(b_buf, buf);
    std::printf("  reader under B: %zu characters, empty=%s\n",
                s.size(), s.empty() ? "yes" : "no");

    // Under contract A the same line computes 0 - 1, which wraps, because
    // size_t is unsigned. Reported, not executed.
    std::printf("  reader under A would ask for    : %zu characters\n",
                static_cast<std::size_t>(a - 1));

    // A non-empty value, where the two contracts differ only by the
    // terminator and both readers agree.
    stored = "hello";
    const std::size_t a2 = query_raw(stored, buf, sizeof buf);
    const std::size_t b2 = get_value(stored, buf, sizeof buf);
    std::printf("\nstored value \"hello\"\n");
    std::printf("  contract A reports : %zu bytes\n", a2);
    std::printf("  contract B reports : %zu bytes\n", b2);
    std::printf("  reader under B     : \"%s\"\n",
                read_unguarded(b2, buf).c_str());

    // The separate claim in the review was that a zero-length string is
    // itself dangerous to construct. It is not.
    const std::string empty(buf, 0);
    std::printf("\nstd::string(buf, 0) built fine, size %zu\n", empty.size());

    return 0;
}
