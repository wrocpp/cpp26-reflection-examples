// verify: gcc-only
// A clone() mixin with no hooks. AutoCloneable has no template parameters. Its clone() deduces the caller's type, then
// reflection lists the concrete classes derived from AutoCloneable in the namespace of that type and copies with the
// one whose type matches the dynamic type of the source. Square, Circle and Tile write nothing: there is no override to
// forget and no type name to repeat.
// Build: g++ -std=c++26 -freflection -O1   (Compiler Explorer: g162)
// Limits, shown at the end of main: the set is the classes of one namespace that are complete where clone() is
// instantiated. A class in another namespace is not in it, and cloning it throws clone_unavailable. The program has no macros and exits 0.
#include <meta>

#include <cstdio>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace shapes {

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
      if constexpr (std::meta::is_type(member) && std::meta::is_class_type(member) &&
                    std::meta::is_base_of_type(^^AutoCloneable, member) && !std::meta::is_abstract_type(member)) {
        using C = typename [:member:];
        if constexpr (std::is_base_of_v<T, C>) {
          if (typeid(self) == typeid(C)) {
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

constexpr int kGrout = 7;  // an arbitrary joint width for Tile

struct Figure : AutoCloneable {
  virtual double area() const = 0;
};

struct Square : Figure {
  double side = 1;
  std::vector<double> history{};

  Square() = default;
  explicit Square(double s) : side{s}, history{s} {}

  double area() const override { return side * side; }
};

struct Circle : Figure {
  double radius = 1;

  Circle() = default;
  explicit Circle(double r) : radius{r} {}

  double area() const override { return std::numbers::pi * radius * radius; }
};

struct Tile : Square {  // nothing to forget: no hooks exist
  using Square::Square;
  int grout = kGrout;
};

}  // namespace shapes

namespace elsewhere {
struct Stray : shapes::Figure {  // derived from Figure, but not in the namespace that clone() searches
  double area() const override { return 0; }
};
}  // namespace elsewhere

constexpr double kSide = 2;

int main() {
  shapes::Tile tile{kSide};
  shapes::Figure& through_base = tile;

  auto from_tile = tile.clone();                  // std::unique_ptr<Tile>
  auto from_base = through_base.clone();          // std::unique_ptr<Figure>, holding a Tile
  auto from_rvalue = std::move(tile).clone();     // moves
  static_assert(std::is_same_v<decltype(from_tile), std::unique_ptr<shapes::Tile>>);
  static_assert(std::is_same_v<decltype(from_base), std::unique_ptr<shapes::Figure>>);

  const shapes::Tile& first = *from_tile;
  const shapes::Figure& second = *from_base;
  const shapes::Tile& third = *from_rvalue;
  std::printf("dynamic types kept: %d %d %d\n", typeid(first) == typeid(shapes::Tile) ? 1 : 0,
              typeid(second) == typeid(shapes::Tile) ? 1 : 0, typeid(third) == typeid(shapes::Tile) ? 1 : 0);
  std::printf("history left in the donor after clone of an rvalue: %zu\n", tile.history.size());

  elsewhere::Stray stray;
  shapes::Figure& stray_base = stray;
  try {
    auto lost = stray_base.clone();
    std::printf("clone of a class outside the namespace returned %s\n", lost ? "an object" : "nothing");
  } catch (const shapes::clone_unavailable& e) {
    std::printf("clone of a class outside the namespace throws clone_unavailable: %s\n", e.what());
  }
}
