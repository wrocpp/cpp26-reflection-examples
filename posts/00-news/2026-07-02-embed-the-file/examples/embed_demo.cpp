// embed_demo.cpp  (~30 lines)
// "#embed: a file is now a literal." C++26 adopts the C23 #embed directive, so
// you can pull a file's bytes straight into your program at translation time --
// no xxd, no codegen step, no build-system glue. The preprocessor expands
// #embed into a comma-separated list of byte values, so it drops directly into
// an initializer.
//
// Here we embed a text file as a byte array, then view those same bytes as a
// string. The array is `static constexpr`, so the contents live in the binary
// with zero runtime loading.
//
// GCC 16.1 ships #embed. On Compiler Explorer the sibling asset.txt is supplied
// alongside the source; locally, just keep asset.txt next to this file.
//
// verify: ce-file: asset.txt
// Compile (GCC 16.1):              g++-16 -std=c++26 embed_demo.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ embed_demo.cpp -o demo

#include <print>
#include <string_view>

int main() {
    static constexpr unsigned char bytes[] = {
        #embed "asset.txt"
    };

    std::string_view text{reinterpret_cast<const char*>(bytes), sizeof(bytes)};
    std::print("embedded {} bytes:\n{}", sizeof(bytes), text);
}
