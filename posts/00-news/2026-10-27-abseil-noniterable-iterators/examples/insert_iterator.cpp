// insert_iterator.cpp
//
// What Abseil's flat_hash_map returns from insert(), and what people do with
// it that they should not.
//
// insert() returns pair<iterator, bool>. The iterator points at the element,
// which is the useful part. It is also, on older Abseil, a perfectly ordinary
// table iterator: increment it and you walk into the rest of the table.
//
// That walk is meaningless. The order is unspecified, the starting point is
// wherever the element hashed to, and a rehash invalidates the whole thing. It
// looks like a sensible range and is not one.
//
// Abseil LTS 20260817.0 (2026-08-18) closed this. The release notes list it
// under breaking changes: "Hashtable iterators returned from insert/emplace
// are now non-iterable (any prior use of iteration was likely a bug)."
//
// This file demonstrates the OLD behaviour, which is what Compiler Explorer's
// Abseil still has. On 20260817.0 the walk below stops immediately, because
// the returned iterator's control pointer is a static two-byte array holding a
// full byte followed by a sentinel, so ++it lands on end().
//
// Compile: g++ -std=c++20 (with abseil). CE's abseil 20260107.1 does not build
// at -std=c++17; its headers reach for std::weak_ordering. Note also that the
// walk below visits a DIFFERENT set of elements at -std=c++20 (99, 10, 7) than
// at -std=c++23 (99, 9), from identical source. That is the clearest possible
// evidence that the walk reports on table layout rather than on your data.
// verify: ce-libs: abseil@202601071
//
// NOTE the library version id is 202601071, NOT the display string
// "20260107.1". Compiler Explorer's /api/libraries endpoint reports both, and
// passing the dotted display version silently fails to attach the library, so
// the build dies on a missing absl header rather than on a bad library name.

#include <cstdio>
#include <string>

#include "absl/container/flat_hash_map.h"

int main() {
    absl::flat_hash_map<int, std::string> m;
    for (int i = 0; i < 12; ++i) {
        m.emplace(i, "value-" + std::to_string(i));
    }

    auto [it, inserted] = m.emplace(99, "value-99");
    std::printf("inserted=%s  key=%d\n", inserted ? "true" : "false", it->first);

    // The useful part: dereferencing the returned iterator. This keeps working
    // on every version, and is the entire reason insert returns an iterator.
    std::printf("*it     = {%d, %s}\n\n", it->first, it->second.c_str());

    // The misuse. Nothing here is a compile error, and on older Abseil it
    // walks an arbitrary suffix of the table in an unspecified order.
    std::printf("walking from the insert iterator to end():\n");
    int seen = 0;
    for (auto walk = it; walk != m.end(); ++walk) {
        std::printf("  visited key %d\n", walk->first);
        ++seen;
    }

    std::printf("\nvisited %d of %zu elements\n", seen, m.size());
    if (seen > 1) {
        std::printf("this is the old behaviour: the walk saw more than the\n"
                    "element that was just inserted, in no defined order\n");
    } else {
        std::printf("this is the 20260817.0 behaviour: ++it went straight to end()\n");
    }
    return 0;
}
