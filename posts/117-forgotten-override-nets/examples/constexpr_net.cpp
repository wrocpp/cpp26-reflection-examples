// verify: gcc-only
// Net 2: a constant-evaluated round trip. clone_keeps_type<T>() clones a T through a const Figure& and compares the
// dynamic type of the copy with the dynamic type of the original. A class that forgot its hooks gets a Square back,
// the comparison is false, and the net reports it. The assertions below state what the net must report, so the
// build passes only when the net catches ForgetfulTile and accepts Square and Tile. No contracts, no macros.
// The concept probes a second thing: whether cloning through the derived type is a constant expression, which the
// compiler rejects for ForgetfulTile because the downcast is undefined.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162; clang2210 builds it too)
#include <cstdio>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

struct Cloneable {
  constexpr virtual ~Cloneable() = default;

  template <class Self>
  constexpr auto clone(this Self&& self) -> std::unique_ptr<std::remove_cvref_t<Self>> {
    using T = std::remove_cvref_t<Self>;
    static_assert(std::is_base_of_v<Cloneable, T>);
    std::unique_ptr<Cloneable> made;
    if constexpr (std::is_lvalue_reference_v<Self> || std::is_const_v<std::remove_reference_t<Self>>) {
      made = static_cast<const Cloneable&>(self).copy_();
    } else {
      made = static_cast<Cloneable&>(self).move_();
    }
    return std::unique_ptr<T>(static_cast<T*>(made.release()));
  }

 protected:
  constexpr Cloneable() = default;
  constexpr Cloneable(const Cloneable&) = default;
  constexpr Cloneable(Cloneable&&) = default;
  constexpr Cloneable& operator=(const Cloneable&) = default;
  constexpr Cloneable& operator=(Cloneable&&) = default;

 private:
  constexpr virtual std::unique_ptr<Cloneable> copy_() const = 0;
  constexpr virtual std::unique_ptr<Cloneable> move_() = 0;
};

struct Figure : Cloneable {
  constexpr virtual double area() const = 0;

 protected:
  constexpr Figure() = default;
  constexpr Figure(const Figure&) = default;
  constexpr Figure(Figure&&) = default;
  constexpr Figure& operator=(const Figure&) = default;
  constexpr Figure& operator=(Figure&&) = default;
};

struct Square : Figure {
  double side = 1;

  constexpr double area() const override { return side * side; }

 private:
  constexpr std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Square>(*this); }
  constexpr std::unique_ptr<Cloneable> move_() override { return std::make_unique<Square>(std::move(*this)); }
};

constexpr int kGrout = 7;  // an arbitrary joint width for the tiles

struct Tile : Square {  // overrides both hooks
  int grout = kGrout;

 private:
  constexpr std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Tile>(*this); }
  constexpr std::unique_ptr<Cloneable> move_() override { return std::make_unique<Tile>(std::move(*this)); }
};

struct ForgetfulTile : Square {  // forgot both overrides
  int grout = kGrout;
};

template <class T>
consteval bool clone_keeps_type() {
  T original{};
  const Figure& base = original;
  auto copy = base.clone();
  const Figure& copied = *copy;
  return typeid(copied) == typeid(original);
}

static_assert(clone_keeps_type<Square>());
static_assert(clone_keeps_type<Tile>());
static_assert(!clone_keeps_type<ForgetfulTile>());

template <class T>
concept clones_in_constant_evaluation = requires { typename std::integral_constant<double, T{}.clone()->area()>; };

static_assert(clones_in_constant_evaluation<Tile>);
static_assert(!clones_in_constant_evaluation<ForgetfulTile>);

int main() {
  Tile tile;
  std::printf("area %g\n", tile.clone()->area());
}
