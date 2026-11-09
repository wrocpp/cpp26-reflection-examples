// verify: gcc-only
// The clone helper from the 2014 CppCodeReviewers article (FigureTests-v5.cpp, commit 4729ab8), with only the
// formatting changed, plus a class that forgets to override clone(): Tile derives from Square and does not.
// Build: g++ -std=c++26 -O0 -Wall -Wextra -Wpedantic -fsanitize=undefined   (Compiler Explorer: g162)
// The compiler prints no warning. Run without an argument the program calls nothing that only Tile has, so it
// exits 0, and UBSan reports the bad downcast on stderr. Run with any argument it also calls a virtual function
// that only Tile declares, which crashes.
#include <cstdio>
#include <memory>
#include <type_traits>

namespace object {
template <typename T>
std::unique_ptr<T> clone(const T& object) {
  using base_type = typename T::base_type;
  static_assert(std::is_base_of<base_type, T>::value, "T object has to derived from T::base_type");
  auto ptr = static_cast<const base_type&>(object).clone();
  return std::unique_ptr<T>(static_cast<T*>(ptr));
}

template <typename T>
auto clone(T* object) -> decltype(clone(*object)) {
  return clone(*object);
}

template <typename T>
struct cloneable {
  using base_type = T;

  virtual ~cloneable() = default;

 protected:
  virtual T* clone() const = 0;

  template <typename X>
  friend std::unique_ptr<X> object::clone(const X&);
};
}  // namespace object

struct Figure : object::cloneable<Figure> {
  virtual double area() const = 0;
};

struct Square : Figure {
  double a = 0;

  Square() = default;
  Square(double a) : a{a} {}

  double area() const override { return a * a; }

 protected:
  Square* clone() const override { return new Square(*this); }
};

constexpr int kGrout = 7;  // an arbitrary joint width for Tile

struct Tile : Square {  // forgot: Tile* clone() const override
  using Square::Square;
  int grout = kGrout;
  virtual int joints() const { return grout; }
};

constexpr double kSide = 2;

int main(int argc, char**) {
  Tile tile{kSide};
  auto copy = object::clone(tile);  // std::unique_ptr<Tile>, but what it owns is a Square
  std::printf("area=%g\n", copy->area());
  if (argc > 1) std::printf("joints=%d\n", copy->joints());
}
