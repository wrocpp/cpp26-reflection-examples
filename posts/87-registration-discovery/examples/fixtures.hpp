// Shared fixtures namespace (header-only so every TU sees all of it).
#pragma once
namespace fixtures {
struct Fixture { int base; };
inline Fixture make_fixture() { return Fixture{40}; }   // resolved by TYPE
inline int seed() { return 42; }                        // resolved by NAME (parameter named seed)
inline int offset() { return 1000; }                    // a second int fixture: type alone is ambiguous
}  // namespace fixtures
