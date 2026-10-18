// verify: gcc-only
// Manual mint, not the pipeline. g++ -std=c++26, once plain and once with -fsanitize=address.
#include <print>
#include <string>
#include <string_view>

// Keep the exit code 0 so the AddressSanitizer run can show its report on Compiler Explorer.
extern "C" const char* __asan_default_options() { return "exitcode=0"; }

std::string_view first_word(std::string s) {  // returns a view into its by-value parameter
  return std::string_view{s}.substr(0, s.find(' '));
}

int main() {
  const std::string original(40, 'x');
  auto w = first_word(original + " tail");  // the parameter is destroyed when the call returns
  std::println("the view still equals the text it was taken from: {}", w == original);
}
