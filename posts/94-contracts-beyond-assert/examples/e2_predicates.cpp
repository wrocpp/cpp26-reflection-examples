// The definitions behind the opaque and pure forms of E2, in their own translation unit.
struct Address {
  int port;
};

bool valid_port(int port) { return port > 0 && port < 65536; }
bool valid_address(const Address& a) { return valid_port(a.port); }
