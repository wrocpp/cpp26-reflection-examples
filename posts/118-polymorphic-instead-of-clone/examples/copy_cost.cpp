// verify: gcc-only
// What it costs to copy a row of three figures (Square, Circle, Tile) in four designs: unique_ptr with the hook-based
// Cloneable mixin of the deducing-this post, std::polymorphic, a std::variant of the three classes that reflection
// builds from the namespace, and unique_ptr with the hookless AutoCloneable mixin. Allocations are counted by a
// replaced operator new.
// Build: g++ -std=c++26 -freflection -O1   (Compiler Explorer: g162)
// The program has no macros and exits 0. With -Wall GCC 16.2 reports -Wmismatched-new-delete on the replaced
// operators, although the pair is matched, so the build above does not use -Wall.
#include <meta>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>
#include <numbers>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

namespace counters {
inline std::size_t allocations = 0;
}  // namespace counters

void* operator new(std::size_t size) {
  ++counters::allocations;
  if (void* p = std::malloc(size)) return p;
  throw std::bad_alloc{};
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

constexpr int kGrout = 7;  // an arbitrary joint width for Tile
constexpr std::size_t kRowSize = 3;

namespace hooks {  // design 1: the hand-written hooks of the deducing-this post

struct Cloneable {
  virtual ~Cloneable() = default;

  template <class Self>
  auto clone(this Self&& self) -> std::unique_ptr<std::remove_cvref_t<Self>> {
    using T = std::remove_cvref_t<Self>;
    std::unique_ptr<Cloneable> made;
    if constexpr (std::is_lvalue_reference_v<Self> || std::is_const_v<std::remove_reference_t<Self>>) {
      made = static_cast<const Cloneable&>(self).copy_();
    } else {
      made = static_cast<Cloneable&>(self).move_();
    }
    return std::unique_ptr<T>(static_cast<T*>(made.release()));
  }

 private:
  virtual std::unique_ptr<Cloneable> copy_() const = 0;
  virtual std::unique_ptr<Cloneable> move_() = 0;
};

struct Figure : Cloneable {
  virtual double area() const = 0;
};

struct Square : Figure {
  double side = 1;
  std::vector<double> history{};
  double area() const override { return side * side; }

 private:
  std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Square>(*this); }
  std::unique_ptr<Cloneable> move_() override { return std::make_unique<Square>(std::move(*this)); }
};

struct Circle : Figure {
  double radius = 1;
  double area() const override { return std::numbers::pi * radius * radius; }

 private:
  std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Circle>(*this); }
  std::unique_ptr<Cloneable> move_() override { return std::make_unique<Circle>(std::move(*this)); }
};

struct Tile : Square {
  int grout = kGrout;

 private:
  std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Tile>(*this); }
  std::unique_ptr<Cloneable> move_() override { return std::make_unique<Tile>(std::move(*this)); }
};

}  // namespace hooks

namespace hookless {  // designs 2 to 4: classes that write no clone code at all

struct clone_unavailable : std::logic_error {  // thrown when no searched class matches the dynamic type
  using std::logic_error::logic_error;
};

struct AutoCloneable {  // a mixin with no template parameters and no hooks
  virtual ~AutoCloneable() = default;

  template <class Self>
  auto clone(this Self&& self) -> std::unique_ptr<std::remove_cvref_t<Self>> {
    using T = std::remove_cvref_t<Self>;
    static constexpr auto members = std::define_static_array(
        std::meta::members_of(std::meta::parent_of(std::meta::dealias(^^T)), std::meta::access_context::unchecked()));
    std::unique_ptr<T> out;
    template for (constexpr auto member : members) {
      if constexpr (std::meta::is_type(member) && std::meta::dealias(member) == member &&
                    std::meta::is_class_type(member) && std::meta::is_complete_type(member) &&
                    std::meta::is_base_of_type(^^AutoCloneable, member) && !std::meta::is_abstract_type(member)) {
        using C = typename [:member:];
        if constexpr (std::is_base_of_v<T, C>) {
          if (!out && typeid(self) == typeid(C)) {  // the first match only
            if constexpr (std::is_lvalue_reference_v<Self> || std::is_const_v<std::remove_reference_t<Self>>) {
              out = std::make_unique<C>(static_cast<const C&>(self));
            } else {
              out = std::make_unique<C>(std::move(static_cast<C&>(self)));
            }
          }
        }
      }
    }
    if (!out) {
      const AutoCloneable& source = self;
      throw clone_unavailable{std::string{"no searched class has the dynamic type "} + typeid(source).name()};
    }
    return out;
  }
};

struct Figure : AutoCloneable {
  virtual double area() const = 0;
};

struct Square : Figure {
  double side = 1;
  std::vector<double> history{};
  double area() const override { return side * side; }
};

struct Circle : Figure {
  double radius = 1;
  double area() const override { return std::numbers::pi * radius * radius; }
};

struct Tile : Square {
  int grout = kGrout;
};

}  // namespace hookless

struct Cost {
  std::size_t allocations;
  std::size_t element_bytes;
  double area;
};

void report(const char* design, const Cost& cost) {
  std::printf("%-44s %zu allocations per row copy, element %zu bytes, total area %g\n", design, cost.allocations,
              cost.element_bytes, cost.area);
}

template <class Ptr>
Cost by_pointer(std::vector<Ptr> row) {
  counters::allocations = 0;
  std::vector<Ptr> copy;
  copy.reserve(row.size());
  for (const auto& figure : row) copy.push_back(figure->clone());
  const std::size_t allocations = counters::allocations;

  double area = 0;
  for (const auto& figure : copy) area += figure->area();
  return {allocations, sizeof(Ptr), area};
}

Cost by_polymorphic() {
  std::vector<std::polymorphic<hookless::Figure>> row;
  row.reserve(kRowSize);
  row.emplace_back(std::in_place_type<hookless::Square>);
  row.emplace_back(std::in_place_type<hookless::Circle>);
  row.emplace_back(std::in_place_type<hookless::Tile>);

  counters::allocations = 0;
  auto copy = row;
  const std::size_t allocations = counters::allocations;

  double area = 0;
  for (const auto& figure : copy) area += figure->area();
  return {allocations, sizeof(std::polymorphic<hookless::Figure>), area};
}

constexpr std::meta::info kBase = ^^hookless::Figure;

consteval std::vector<std::meta::info> concrete_figures() {
  std::vector<std::meta::info> found;
  for (auto member : std::meta::members_of(^^hookless, std::meta::access_context::unchecked())) {
    if (std::meta::is_type(member) && std::meta::dealias(member) == member && std::meta::is_class_type(member) &&
        std::meta::is_complete_type(member) && member != kBase && std::meta::is_base_of_type(kBase, member) &&
        !std::meta::is_abstract_type(member)) {
      found.push_back(member);
    }
  }
  return found;
}

using AnyFigure = [:std::meta::substitute(^^std::variant, concrete_figures()):];

Cost by_variant() {
  std::vector<AnyFigure> row;
  row.reserve(kRowSize);
  row.emplace_back(hookless::Square{});
  row.emplace_back(hookless::Circle{});
  row.emplace_back(hookless::Tile{});

  counters::allocations = 0;
  auto copy = row;
  const std::size_t allocations = counters::allocations;

  double area = 0;
  for (const auto& figure : copy) area += std::visit([](const auto& f) { return f.area(); }, figure);
  return {allocations, sizeof(AnyFigure), area};
}

int main() {
  std::vector<std::unique_ptr<hooks::Figure>> hooked;
  hooked.reserve(kRowSize);
  hooked.push_back(std::make_unique<hooks::Square>());
  hooked.push_back(std::make_unique<hooks::Circle>());
  hooked.push_back(std::make_unique<hooks::Tile>());

  std::vector<std::unique_ptr<hookless::Figure>> hookless_row;
  hookless_row.reserve(kRowSize);
  hookless_row.push_back(std::make_unique<hookless::Square>());
  hookless_row.push_back(std::make_unique<hookless::Circle>());
  hookless_row.push_back(std::make_unique<hookless::Tile>());

  report("unique_ptr, hand-written hooks", by_pointer(std::move(hooked)));
  report("unique_ptr, hookless reflection mixin", by_pointer(std::move(hookless_row)));
  report("polymorphic", by_polymorphic());
  report("variant of the reflected classes", by_variant());
}
