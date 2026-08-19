#include <print>
#include <vector>
#include <meta>

template <class... Ts>
void print_all(Ts... ts) {
    template for (auto t : {ts...}) {
        std::println("{} = {}",
                     std::meta::display_string_of(^^decltype(t)),
                     t);
    }
}

int main() {
    print_all(1.2, "abc", 3, std::vector<int>({2,3}));
    return EXIT_SUCCESS;
}
