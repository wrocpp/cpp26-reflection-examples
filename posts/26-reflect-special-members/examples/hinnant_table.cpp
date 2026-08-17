// hinnant_table.cpp
//
// Howard Hinnant's table says which special member functions the compiler
// still generates once you declare one yourself. Everyone has seen it. Almost
// nobody checks it.
//
// C++26 reflection can check it. members_of gives every member the compiler
// actually produced, is_function separates the methods, and is_deleted tells
// you which ones were generated only to be deleted. Counting them turns the
// table from something you remember into something you print.
//
// Approach follows Lieven de Cock, "C++ Reflection: Verifying
// Compiler-generated Functions", Overload 194, August 2026.
//
// Each struct below carries one ordinary method, doSomething, so the special
// member count is (functions - 1).
//
// Compile: clang-p2996, -std=c++26 -freflection-latest -stdlib=libc++
// verify: clang-only
// verify: clang-options: -std=c++26 -freflection-latest -stdlib=libc++ -O1

#include <experimental/meta>
#include <cstdio>

template <typename T>
consteval int live_functions() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    int n = 0;
    template for (constexpr auto m : std::define_static_array(std::meta::members_of(^^T, ctx))) {
        if (std::meta::is_function(m) && !std::meta::is_deleted(m)) { ++n; }
    }
    return n;
}

template <typename T>
consteval int deleted_functions() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    int n = 0;
    template for (constexpr auto m : std::define_static_array(std::meta::members_of(^^T, ctx))) {
        if (std::meta::is_function(m) && std::meta::is_deleted(m)) { ++n; }
    }
    return n;
}

// Row 1: declare nothing. All six special members are generated.
struct Row1 { int x{}; int y{}; void doSomething() {} };

// Row 2: any constructor at all costs you the default constructor.
struct Row2 { int x{}; int y{}; Row2(int); void doSomething() {} };

// Row 3: declaring the default constructor yourself gets it back.
struct Row3 { int x{}; int y{}; Row3(); void doSomething() {} };

// Row 4: the one that surprises people. Defaulting the destructor reads as a
// no-op and removes both move operations.
struct Row4 { int x{}; int y{}; ~Row4() = default; void doSomething() {} };

// Row 5: a copy constructor costs the default constructor and both moves.
struct Row5 { int x{}; int y{}; Row5(const Row5&); void doSomething() {} };

// Row 6: a copy assignment operator costs both moves.
struct Row6 { int x{}; int y{}; Row6& operator=(const Row6&); void doSomething() {} };

// Row 7: a move constructor costs the default constructor and move assignment,
// and the copy operations are generated as DELETED rather than absent.
struct Row7 { int x{}; int y{}; Row7(Row7&&); void doSomething() {} };

// Row 8: a move assignment operator does the same to the copies.
struct Row8 { int x{}; int y{}; Row8& operator=(Row8&&); void doSomething() {} };

// `ordinary` is how many declared functions are NOT special members and so
// must come out of the count. Every row has doSomething. Row 2 also declares
// Row2(int), which is a constructor but not a special member, so it has two.
// Getting this wrong inflates Row 2 to 6/6 and quietly contradicts the table
// the program is supposed to be checking.
template <typename T>
void report(const char* label, const char* declared, int ordinary = 1) {
    constexpr int live = live_functions<T>();
    constexpr int gone = deleted_functions<T>();
    std::printf("  %-5s %-26s special=%d/6  deleted=%d\n",
                label, declared, live - ordinary, gone);
}

int main() {
    std::printf("declaring one special member changes what you get:\n\n");
    report<Row1>("Row1", "nothing");
    report<Row2>("Row2", "some constructor", 2);   // doSomething + Row2(int)
    report<Row3>("Row3", "default constructor");
    report<Row4>("Row4", "~T() = default");
    report<Row5>("Row5", "copy constructor");
    report<Row6>("Row6", "copy assignment");
    report<Row7>("Row7", "move constructor");
    report<Row8>("Row8", "move assignment");
    std::printf("\ndeleted counts are copy operations the compiler emitted only to delete\n");
    return 0;
}
