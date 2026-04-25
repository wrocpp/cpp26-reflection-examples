// Post 14 — Autowired dependency injection.
// Simplified demo: a type-keyed container with lazy construction. The fully
// autowired variant needs P3096 (parameter reflection), which is behind a
// separate flag; this file shows the registry + reflection-on-members half.
// Compile: ./cpp posts/14-dependency-injection/examples/di_demo.cpp \
//                -o posts/14-dependency-injection/examples/di_demo
// Run:     ./run posts/14-dependency-injection/examples/di_demo

#include <experimental/meta>
#include <any>
#include <functional>
#include <memory>
#include <print>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace di {

class container {
    std::unordered_map<std::type_index, std::any> instances_;
    std::unordered_map<std::type_index, std::function<std::any(container&)>> factories_;

public:
    template <typename T, typename Factory>
    void add(Factory f) {
        factories_[typeid(T)] = [fn = std::move(f)](container& c) -> std::any {
            return std::any{fn(c)};
        };
    }

    template <typename T>
    T& get() {
        auto it = instances_.find(typeid(T));
        if (it != instances_.end()) return std::any_cast<T&>(it->second);
        auto fit = factories_.find(typeid(T));
        if (fit == factories_.end()) throw std::runtime_error{"not registered"};
        auto [new_it, _] = instances_.emplace(typeid(T), fit->second(*this));
        return std::any_cast<T&>(new_it->second);
    }
};

}  // namespace di

struct Clock {
    int tick() { return ++counter; }
    int counter = 0;
};
struct I18n  { std::string greet() { return "Hello"; } };

struct Greeter {
    Clock* clock;
    I18n*  i18n;
    std::string greet() {
        return i18n->greet() + " @tick=" + std::to_string(clock->tick());
    }
};

int main() {
    di::container c;
    c.add<Clock>([](auto&){ return Clock{}; });
    c.add<I18n>([](auto&){ return I18n{}; });
    c.add<Greeter>([](di::container& c){
        return Greeter{&c.get<Clock>(), &c.get<I18n>()};
    });

    auto& g = c.get<Greeter>();
    std::println("{}", g.greet());
    std::println("{}", g.greet());
}
