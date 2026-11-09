// verify: gcc-only
// std::polymorphic<Figure> copies the dynamic type of what it holds, so the classes need no clone(), no hooks and no
// override to forget. Same classes as the clone posts, without the clone machinery.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162; the library must provide std::polymorphic. libc++ trunk did
//        not on 2026-11-09, and the include of <memory> then gives no std::polymorphic.)
// The program has no macros. The conversion that does not exist is a static_assert on a type trait. It exits 0.
#include <cstddef>
#include <cstdio>
#include <memory>
#include <numbers>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

constexpr int kGrout = 7;  // an arbitrary joint width for Tile

struct Figure {
  constexpr virtual ~Figure() = default;
  constexpr virtual double area() const = 0;
};

struct Square : Figure {
  double side = 1;
  std::vector<double> history{};

  constexpr Square() = default;
  constexpr explicit Square(double s) : side{s}, history{s} {}

  constexpr double area() const override { return side * side; }
};

struct Circle : Figure {
  double radius = 1;

  constexpr Circle() = default;
  constexpr explicit Circle(double r) : radius{r} {}

  constexpr double area() const override { return std::numbers::pi * radius * radius; }
};

struct Tile : Square {  // nothing to forget
  using Square::Square;
  int grout = kGrout;
};

constexpr double kSide = 2;
constexpr double kRadius = 1;

static_assert([] {
  std::polymorphic<Figure> original{std::in_place_type<Tile>, kSide};
  auto copy = original;
  const Figure& inside = *copy;
  return typeid(inside) == typeid(Tile) && &*copy != &*original;
}());

// A std::polymorphic<Square> does not convert to a std::polymorphic<Figure>. With clone() the same step is a
// conversion of a unique_ptr.
static_assert(!std::is_convertible_v<std::polymorphic<Square>, std::polymorphic<Figure>>);
static_assert(std::is_convertible_v<std::unique_ptr<Square>, std::unique_ptr<Figure>>);

const Figure& held(const std::polymorphic<Figure>& p) { return *p; }

int main() {
  std::vector<std::polymorphic<Figure>> row;
  row.emplace_back(std::in_place_type<Square>, kSide);
  row.emplace_back(std::in_place_type<Circle>, kRadius);
  row.emplace_back(std::in_place_type<Tile>, kSide);

  auto copy = row;  // deep copy: three new objects of the same three dynamic types
  int same_type = 0;
  int distinct = 0;
  for (std::size_t i = 0; i < row.size(); ++i) {
    same_type += typeid(held(copy[i])) == typeid(held(row[i])) ? 1 : 0;
    distinct += &held(copy[i]) != &held(row[i]) ? 1 : 0;
  }
  std::printf("copies with the same dynamic type: %d of %zu, distinct objects: %d of %zu\n", same_type, row.size(),
              distinct, row.size());
  std::printf("sizeof(std::polymorphic<Figure>) = %zu, sizeof(std::unique_ptr<Figure>) = %zu\n",
              sizeof(std::polymorphic<Figure>), sizeof(std::unique_ptr<Figure>));

  auto taken = std::move(row[0]);
  std::printf("source valueless after the move: %s, the moved-to value holds area %g\n",
              row[0].valueless_after_move() ? "yes" : "no", taken->area());
}
