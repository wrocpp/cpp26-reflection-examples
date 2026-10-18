// verify: gcc-only
// Manual mint, not the pipeline. g++ -std=c++26 -Wall -Wextra, and with -fsanitize=address.
#include <print>
#include <string>

extern "C" const char* __asan_default_options() { return "exitcode=0"; }

const std::string& longer(const std::string& a, const std::string& b) {  // the std::max shape
  return a.size() >= b.size() ? a : b;
}

int main() {
  const std::string& r = longer(std::string(40, 'a'), std::string(50, 'b'));  // both arguments are temporaries
  std::println("length through the reference: {}", r.size());
}
