// The definition behind the declared-only form of E3.
struct Address {
  int port;
};

int connect_to(const Address& a) post(r : r == 0 || r == -1) { return a.port == 1 ? -1 : 0; }
