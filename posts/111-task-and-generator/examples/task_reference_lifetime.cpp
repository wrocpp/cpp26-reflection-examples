// CP.53 for tasks: a reference parameter of a coroutine outlives its argument.
// exec::task is NVIDIA stdexec's reference implementation, not std::execution::task.
#include <stdexec/execution.hpp>
#include <exec/task.hpp>

#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>

constexpr std::size_t text_length = 100;

#ifdef BY_REFERENCE
exec::task<std::size_t> length_of(const std::string& text) {
  co_await stdexec::just();
  co_return text.size();
}
#else
exec::task<std::size_t> length_of(std::string text) {
  co_await stdexec::just();
  co_return text.size();
}
#endif

int main() {
  auto task = length_of(std::string(text_length, 'x'));  // the temporary dies at the end of this statement
  auto [length] = stdexec::sync_wait(std::move(task)).value();
  std::printf("length %zu\n", length);
}
