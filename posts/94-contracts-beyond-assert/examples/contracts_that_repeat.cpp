// Where a precondition is checked, counted. The same bad port goes down two call chains under the observe
// semantic, which reports each violation and carries on, so the program exits 0.
// GCC 16.2: g++ -std=c++26 -Wall -Wextra -fcontract-evaluation-semantic=observe contracts_that_repeat.cpp
//   plain ints: every function states the precondition it needs, so one bad value is reported by each of them.
//   a Port type: the value is checked where it is made, and the later functions have nothing left to check.
#include <contracts>
#include <cstdio>
#include <optional>

namespace {
int reports = 0;
}

void handle_contract_violation(const std::contracts::contract_violation&) { ++reports; }

constexpr bool valid_port_number(int port) { return port > 0 && port < 65536; }

namespace plain {
struct Address {
  int port;
};

constexpr bool valid_address(const Address& a) { return valid_port_number(a.port); }

[[gnu::noinline]] Address make_address(int port) pre(valid_port_number(port)) { return Address{port}; }
[[gnu::noinline]] int connect_to(const Address& a) pre(valid_address(a)) { return a.port == 1 ? -1 : 0; }
[[gnu::noinline]] int send_to(const Address& a) pre(valid_address(a)) { return a.port + 1; }
[[gnu::noinline]] int close_to(const Address& a) pre(valid_address(a)) { return a.port - 1; }

int session(int number) {
  const Address a = make_address(number);
  return connect_to(a) + send_to(a) + close_to(a);
}
}  // namespace plain

namespace typed {
class Port {
 public:
  explicit Port(int number) pre(valid_port_number(number)) : number_(number) {}
  static std::optional<Port> parse(int number) {
    if (!valid_port_number(number)) return std::nullopt;
    return Port{number};
  }
  int number() const { return number_; }

 private:
  int number_;
};

struct Address {
  Port port;
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
}  // namespace typed

int main() {
  constexpr int good = 8080;
  constexpr int bad = 70000;

  const int plain_good = (reports = 0, plain::session(good), reports);
  const int plain_bad = (reports = 0, plain::session(bad), reports);
  const int typed_good = (reports = 0, typed::session(good), reports);
  const int typed_bad = (reports = 0, typed::session(bad), reports);
  std::printf("plain ints: port %d -> %d reports, port %d -> %d reports\n", good, plain_good, bad, plain_bad);
  std::printf("Port type:  port %d -> %d reports, port %d -> %d reports\n", good, typed_good, bad, typed_bad);

  reports = 0;
  const typed::Port unchecked{bad};  // building a Port directly still has its own precondition
  std::printf("Port{%d} built directly -> %d report (number %d)\n", bad, reports, unchecked.number());
  return 0;
}
