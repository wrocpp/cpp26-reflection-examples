// E2: does the check inside make_address (its postcondition) make the check at connect (its precondition)
// unnecessary? Both are the same predicate on the same value. The predicates come in three forms, chosen with
// -DPRED=opaque|pure|inline, and the question is how many calls to valid_address main still makes.
#include <cstdio>

struct Address {
  int port;
};

#if PRED_OPAQUE
bool valid_port(int);                // defined in another translation unit: the optimizer sees nothing
bool valid_address(const Address&);
#elif PRED_PURE
[[gnu::pure]] bool valid_port(int);  // defined elsewhere, but promised to have no effect
[[gnu::pure]] bool valid_address(const Address&);
#else
inline bool valid_port(int port) { return port > 0 && port < 65536; }  // visible: the optimizer can reason about it
inline bool valid_address(const Address& a) { return valid_port(a.port); }
#endif

// -DINLINE_ALL=1 forces both functions into main, to see what the optimizer does when it can see every check.
#if INLINE_ALL
#define FN [[gnu::always_inline]] inline
#else
#define FN
#endif

FN Address make_address(int port) pre(valid_port(port)) post(r : valid_address(r)) { return Address{port}; }

FN int connect_to(const Address& a) pre(valid_address(a)) post(r : r == 0 || r == -1) { return a.port == 1 ? -1 : 0; }

// -DCONST_PORT=1 passes the literal 8080; otherwise the port depends on argc and is unknown at compile time.
#if CONST_PORT
#define PORT_ARG 8080
#else
#define PORT_ARG (argc + 8000)
#endif

int main(int argc, char**) {
  const int result = connect_to(make_address(PORT_ARG));
  std::printf("%d\n", result);
  return 0;
}
