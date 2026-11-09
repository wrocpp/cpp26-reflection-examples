// verify: gcc-only
// A clone() that returns std::unique_ptr<Derived> without covariant returns, a friend or a CRTP base. The mixin
// Cloneable has no template parameters: one member function template with an explicit object parameter (deducing
// this) deduces the caller's type, and two private virtual hooks, which return std::unique_ptr<Cloneable>, do the
// virtual part. Replaces object::clone and object::cloneable of the 2014 article.
// Build: g++ -std=c++26 -O1 -Wall -Wextra -Wpedantic   (Compiler Explorer: g162; clang2210 builds it too)
// The program has no macros. The lines that must not compile are static_asserts on type traits, so the build
// passing proves they are rejected. It exits 0.
#include <cstddef>
#include <cstdio>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

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

 protected:  // the implicit operations of Figure are public, so they are made protected here
  constexpr Figure() = default;
  constexpr Figure(const Figure&) = default;
  constexpr Figure(Figure&&) = default;
  constexpr Figure& operator=(const Figure&) = default;
  constexpr Figure& operator=(Figure&&) = default;
};

struct Square : Figure {
  double side = 1;
  std::vector<double> history{};

  constexpr Square() = default;
  constexpr explicit Square(double s) : side{s}, history{s} {}

  constexpr double area() const override { return side * side; }

 private:
  constexpr std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Square>(*this); }
  constexpr std::unique_ptr<Cloneable> move_() override { return std::make_unique<Square>(std::move(*this)); }
};

// Assigning through the base slices. A protected, defaulted operator in the base blocks it from outside and keeps
// the derived class assignable. A deleted operator in the base, even with a reason, deletes the derived one too.
static_assert(!std::is_assignable_v<Figure&, const Figure&>);
static_assert(std::is_copy_assignable_v<Square> && std::is_move_assignable_v<Square>);

struct DeletedBase {
  virtual ~DeletedBase() = default;
  DeletedBase& operator=(const DeletedBase&) = delete("assigning through a base slices; use clone()");
};
struct DeletedDerived : DeletedBase {};
static_assert(!std::is_copy_assignable_v<DeletedDerived> && !std::is_move_assignable_v<DeletedDerived>);

constexpr double kSide = 4;
constexpr double kOtherSide = 2;
constexpr std::size_t kRowSize = 3;

static_assert([] {
  Square square{kSide};
  Figure& figure = square;
  return figure.clone()->area() + Square{kOtherSide}.clone()->area();
}() == kSide * kSide + kOtherSide * kOtherSide);

int main() {
  Square square{kSide};
  Figure& figure = square;
  const Square frozen{kSide};

  auto from_square = square.clone();
  auto from_figure = figure.clone();
  auto from_const = frozen.clone();
  auto from_rvalue = Square{kOtherSide}.clone();
  static_assert(std::is_same_v<decltype(from_square), std::unique_ptr<Square>>);
  static_assert(std::is_same_v<decltype(from_figure), std::unique_ptr<Figure>>);
  static_assert(std::is_same_v<decltype(from_const), std::unique_ptr<Square>>);
  static_assert(std::is_same_v<decltype(from_rvalue), std::unique_ptr<Square>>);

  Square donor{kSide};
  auto moved = std::move(donor).clone();    // rvalue: move_()
  auto copied = std::move(frozen).clone();  // const rvalue: still a copy
  std::printf("history left in the donor after clone of an rvalue: %zu\n", donor.history.size());
  std::printf("history left in the const source after clone of a const rvalue: %zu\n", frozen.history.size());
  std::printf("areas %g %g %g %g\n", from_square->area(), from_figure->area(), moved->area(), copied->area());

  Square left{kOtherSide};
  Square right{kSide};
  left = right;  // a derived class stays assignable
  std::vector<Square> row(kRowSize);
  row.erase(row.begin());
  std::printf("assigned area %g, row size %zu\n", left.area(), row.size());
}
