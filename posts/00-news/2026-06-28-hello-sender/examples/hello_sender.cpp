// hello_sender.cpp  (~35 lines)
// "The third C++26 pillar": std::execution -- senders, receivers, schedulers
// (P2300), the composable async model that shipped in C++26 alongside
// reflection and contracts. This is the smallest sender pipeline that does
// real work: a value, a transform, and a blocking wait.
//
// A sender is a LAZY description of async work. `just(21)` is a sender that
// will produce 21; `then(f)` is an adaptor that runs f on the value; the
// `operator|` chains them. Nothing runs until `sync_wait` drives the pipeline
// to completion on the current thread and hands back the result.
//
// <std/execution> is not yet in libstdc++/libc++ shipping headers (June 2026),
// so this uses the NVIDIA stdexec reference implementation -- the same code
// the wording was standardised from. Namespaces map 1:1 to std::execution.
//
// verify: ce-libs: stdexec@trunk
// Compile (GCC 16.1):              g++-16 -std=c++26 -I<stdexec>/include hello_sender.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ -I<stdexec>/include hello_sender.cpp -o demo

#include <stdexec/execution.hpp>
#include <print>

int main() {
    // Build a lazy pipeline: produce 21, then double it.
    auto work = stdexec::just(21)
              | stdexec::then([](int x) { return x * 2; });

    // Drive it to completion on this thread; unwrap the single result.
    auto [result] = stdexec::sync_wait(work).value();

    std::println("doubled = {}", result);
}
