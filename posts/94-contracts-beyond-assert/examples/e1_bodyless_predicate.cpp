// E1: a postcondition that names a predicate nobody defines. Is it a promise, or a link error?
struct Address {
  int port;
};

bool valid_port(int);                // declared, never defined
bool valid_address(const Address&);  // declared, never defined

Address make_address(int port) pre(valid_port(port)) post(r : valid_address(r)) { return Address{port}; }

int main() {
  const Address a = make_address(8080);
  return a.port == 8080 ? 0 : 1;
}
