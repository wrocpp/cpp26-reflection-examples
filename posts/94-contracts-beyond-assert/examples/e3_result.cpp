// E3: connect_to promises its result is 0 or -1. Do `r < 0`, `r == -1` and `r != 0` then compile to the same code,
// and does `r <= 0` or `r > 0` draw a warning? -DVISIBLE=1 puts the definition in this translation unit;
// otherwise it is declared only, with the postcondition, and defined in e3_connect.cpp.
struct Address {
  int port;
};

#if VISIBLE
int connect_to(const Address& a) post(r : r == 0 || r == -1) { return a.port == 1 ? -1 : 0; }
#else
int connect_to(const Address& a) post(r : r == 0 || r == -1);
#endif

// Each function asks the same question of the same result in a different way.
bool failed_lt(const Address& a) { return connect_to(a) < 0; }
bool failed_eq(const Address& a) { return connect_to(a) == -1; }
bool failed_ne(const Address& a) { return connect_to(a) != 0; }
bool failed_le(const Address& a) { return !(connect_to(a) <= 0) ? true : false; }  // r > 0: can never be true
bool ok_le(const Address& a) { return connect_to(a) <= 0; }                         // always true
