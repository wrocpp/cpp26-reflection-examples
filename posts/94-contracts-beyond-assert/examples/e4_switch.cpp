// E4: what warns today about a switch that misses a case, or names one that cannot happen?
// An int errno gets no help. An enum class result gets exhaustiveness from -Wswitch, with no contract at all.
#include <cerrno>

enum class Connect { ok, refused, timed_out, unreachable };

int by_errno(int e) {
  switch (e) {  // errno values this call can never produce are named here, and ENETUNREACH is missing
    case 0: return 0;
    case ECONNREFUSED: return 1;
    case ETIMEDOUT: return 2;
    case EINVAL: return 3;  // the postcondition of connect_to says r is 0 or -1: this can never happen
  }
  return -1;
}

int by_enum(Connect c) {
  switch (c) {  // unreachable is missing: -Wswitch names it
    case Connect::ok: return 0;
    case Connect::refused: return 1;
    case Connect::timed_out: return 2;
  }
  return -1;
}

int by_enum_with_stray_case(Connect c) {
  switch (c) {
    case Connect::ok: return 0;
    case Connect::refused: return 1;
    case Connect::timed_out: return 2;
    case Connect::unreachable: return 3;
    case static_cast<Connect>(7): return 4;  // not an enumerator: -Wswitch says so
  }
  return -1;
}
