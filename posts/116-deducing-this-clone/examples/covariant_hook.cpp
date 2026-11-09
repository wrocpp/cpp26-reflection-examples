// verify: gcc-only
// GCC PR126324: constant evaluation of a virtual call whose override has a covariant return type is an internal
// compiler error in GCC 13.1 to 16.2 (fixed on trunk). This is the covariant form that the 2014 design uses: a
// raw-pointer hook, Square* copy() const override, called through a Figure& and stored in a Figure*.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: gsnapshot builds and runs it; g151, g161 and g162 stop with an
//        internal compiler error. The hook of deducing_this_clone.cpp returns a smart pointer and is not covariant.)
#include <cstdio>
#include <memory>

struct RawCloneable {  // the other mixin: a raw owning pointer, because only a raw pointer can be covariant
  constexpr virtual ~RawCloneable() = default;
  constexpr virtual RawCloneable* copy() const = 0;
};

struct Figure : RawCloneable {
  constexpr virtual double area() const = 0;
  constexpr Figure* copy() const override = 0;
};

struct Square : Figure {
  double side = 1;

  constexpr double area() const override { return side * side; }
  constexpr Square* copy() const override { return new Square(*this); }
};

constexpr double kSide = 4;

static_assert([] {
  Square square;
  square.side = kSide;
  const Figure& figure = square;
  Figure* raw = figure.copy();  // the covariant result is stored in a Figure*
  std::unique_ptr<Figure> copy{raw};
  return copy->area();
}() == kSide * kSide);

int main() { std::puts("constant evaluation of the covariant virtual call passed"); }
