// verify: gcc-only
// Net 1: a contract_assert in clone() compares the dynamic type of the source with the dynamic type of the object the
// hook made, before the downcast. The mixin Cloneable carries the check; Figure, Square and Tile are the classes of
// the other examples. ForgetfulTile forgets both hooks. A replacement contract violation handler counts violations.
// The check cannot sit on the virtual hooks: GCC 16.2 rejects a contract on a virtual function.
// Build: g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe -O1   (Compiler Explorer: g162)
// With observe the handler is called, the program goes on and exits 0. enforce, quick_enforce and ignore are
// chosen with the same flag. Without -fcontracts the link fails: handle_contract_violation is undefined.
#include <contracts>

#include <cstdio>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace {
int violations = 0;
}

void handle_contract_violation(const std::contracts::contract_violation& violation) {
  ++violations;
  std::printf("contract violation: %s\n", violation.comment());
}

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
    {
      const Cloneable& made_ref = *made;
      contract_assert(typeid(made_ref) == typeid(self));  // same dynamic type as the source, before the downcast
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

int main() {
  Square square;
  Tile tile;
  ForgetfulTile forgetful;
  Figure& through_base = forgetful;

  auto a = square.clone();
  auto b = tile.clone();
  auto c = forgetful.clone();       // Self = ForgetfulTile&: the check fails
  auto d = through_base.clone();    // Self = Figure&: the check fails here too
  std::printf("violations recorded: %d\n", violations);
}
