// E5: a constant port. Is a violated precondition already a compile-time error when the call is a constant
// expression? Build with -DPORT=8080 and -DPORT=70000, at each evaluation semantic.
#ifndef PORT
#define PORT 8080
#endif

struct Address {
  int port;
};

constexpr bool valid_port(int port) { return port > 0 && port < 65536; }

constexpr Address make_address(int port) pre(valid_port(port)) { return Address{port}; }

constexpr Address fixed = make_address(PORT);  // a constant expression: checked while compiling

int main() { return fixed.port == PORT ? 0 : 1; }
