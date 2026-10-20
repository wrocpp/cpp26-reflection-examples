// E6: where does the check live? With plain ints every function that needs a valid port or address says so in its
// own precondition. With types, the check is made once, where the value is created, and the rest need none.
// -DTYPED=1 builds the second form. Both do the same work: make an address, connect, send, close.
#include <optional>

constexpr bool valid_port_number(int port) { return port > 0 && port < 65536; }

#if TYPED

class Port {
 public:
  explicit Port(int number) pre(valid_port_number(number)) : number_(number) {}
  static std::optional<Port> parse(int number) {  // the boundary: untrusted input becomes a Port or nothing
    if (!valid_port_number(number)) return std::nullopt;
    return Port{number};
  }
  int number() const { return number_; }

 private:
  int number_;
};

struct Address {
  Port port;  // an Address can only be built from a Port, so it cannot hold a bad one
};

[[gnu::noinline]] Address make_address(Port port) { return Address{port}; }
[[gnu::noinline]] int connect_to(const Address& a) { return a.port.number() == 1 ? -1 : 0; }
[[gnu::noinline]] int send_to(const Address& a) { return a.port.number() + 1; }
[[gnu::noinline]] int close_to(const Address& a) { return a.port.number() - 1; }

int session(int number) {
  const auto port = Port::parse(number);
  if (!port) return -1;
  const Address a = make_address(*port);
  return connect_to(a) + send_to(a) + close_to(a);
}

#else

struct Address {
  int port;
};

constexpr bool valid_address(const Address& a) { return valid_port_number(a.port); }

[[gnu::noinline]] Address make_address(int port) pre(valid_port_number(port)) { return Address{port}; }
[[gnu::noinline]] int connect_to(const Address& a) pre(valid_address(a)) { return a.port == 1 ? -1 : 0; }
[[gnu::noinline]] int send_to(const Address& a) pre(valid_address(a)) { return a.port + 1; }
[[gnu::noinline]] int close_to(const Address& a) pre(valid_address(a)) { return a.port - 1; }

int session(int number) {
  if (!valid_port_number(number)) return -1;
  const Address a = make_address(number);
  return connect_to(a) + send_to(a) + close_to(a);
}

#endif

int main(int argc, char**) { return session(argc + 8000) > 0 ? 0 : 1; }
