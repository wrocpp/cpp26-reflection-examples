// Replacing GCC 16.2's default violation handler (no -lstdc++exp needed).
// g++ -std=c++26 -Wall -Wextra -fcontract-evaluation-semantic=observe custom_handler.cpp
// Under observe the handler returns and the program continues (exit 0).
// Under the default enforce the handler runs and then std::terminate is called.
#include <contracts>
#include <cstdio>

void handle_contract_violation(const std::contracts::contract_violation& cv)
{
    std::fprintf(stderr, "custom handler: %s | semantic %d\n", cv.comment(),
                 static_cast<int>(cv.semantic()));
}

int main()
{
    int i = 5;
    contract_assert(i < 3);
    std::fprintf(stderr, "after the contract_assert\n");
}
