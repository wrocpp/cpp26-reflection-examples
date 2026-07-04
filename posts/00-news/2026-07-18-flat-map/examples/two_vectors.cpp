// two_vectors.cpp  (~28 lines)
// C++23's std::flat_map (P0429) is not a tree with better marketing: it is an
// ADAPTOR over two sequences -- a sorted vector of keys and a parallel vector
// of values. The keys()/values() observers let you see the layout directly,
// and the sorted_unique constructor adopts pre-sorted containers with no sort,
// no tree building, no per-node allocation.
//
// Compile (GCC 16.1):   g++ -std=c++23 -O2 two_vectors.cpp
// Compile (clang/libc++): clang++ -std=c++23 -stdlib=libc++ -O2 two_vectors.cpp

#include <flat_map>
#include <print>
#include <string>
#include <vector>

int main() {
    // A flat_map is not a tree; it is an adaptor over TWO sequences.
    std::flat_map<std::string, int> ports{
        {"https", 443}, {"ssh", 22}, {"dns", 53}, {"http", 80},
    };

    std::println("lookup ssh -> {}", ports.at("ssh"));

    // The underlying storage is directly observable: sorted keys, parallel values.
    std::println("keys:   {}", ports.keys());
    std::println("values: {}", ports.values());

    // The adaptor nature is the API: hand it the two underlying containers,
    // pre-sorted, and construction adopts them as-is -- no tree, no sort.
    std::vector<int> ids{1, 2, 3};
    std::vector<int> scores{10, 20, 30};
    std::flat_map<int, int> fm{std::sorted_unique, std::move(ids), std::move(scores)};
    std::println("bulk: {} entries, first={}", fm.size(), fm.begin()->second);
}
