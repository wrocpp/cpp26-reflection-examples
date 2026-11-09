// verify: gcc-only
// Net 3: reflection lists every concrete class derived from Figure in a namespace, reports the ones that do not
// declare an override of both hooks (a lint), and runs a clone round trip on each one (generated tests), so a new
// class is covered without being added to a list. ForgetfulTile is in the namespace on purpose: the program checks
// that the lint finds exactly that class and that the generated test fails for exactly that class. No macros.
// Build: g++ -std=c++26 -freflection -O1   (Compiler Explorer: g162;
//        clang_bb_p2996 with -std=c++26 -freflection-latest -stdlib=libc++ builds it too)
// The program prints its findings and exits 0 when they are the expected ones.
// Limits: it sees one namespace of one translation unit, and a generated test needs a default constructor.
#include <meta>

#include <cstdio>
#include <memory>
#include <print>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace shapes {

struct Cloneable {
  virtual ~Cloneable() = default;

  template <class Self>
  auto clone(this Self&& self) -> std::unique_ptr<std::remove_cvref_t<Self>> {
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
  Cloneable() = default;
  Cloneable(const Cloneable&) = default;
  Cloneable(Cloneable&&) = default;
  Cloneable& operator=(const Cloneable&) = default;
  Cloneable& operator=(Cloneable&&) = default;

 private:
  virtual std::unique_ptr<Cloneable> copy_() const = 0;
  virtual std::unique_ptr<Cloneable> move_() = 0;
};

struct Figure : Cloneable {
  virtual double area() const = 0;

 protected:
  Figure() = default;
  Figure(const Figure&) = default;
  Figure(Figure&&) = default;
  Figure& operator=(const Figure&) = default;
  Figure& operator=(Figure&&) = default;
};

struct Square : Figure {
  double side = 1;

  double area() const override { return side * side; }

 private:
  std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Square>(*this); }
  std::unique_ptr<Cloneable> move_() override { return std::make_unique<Square>(std::move(*this)); }
};

int kGrout = 7;  // an arbitrary joint width for the tiles

struct Tile : Square {  // overrides both hooks
  int grout = kGrout;

 private:
  std::unique_ptr<Cloneable> copy_() const override { return std::make_unique<Tile>(*this); }
  std::unique_ptr<Cloneable> move_() override { return std::make_unique<Tile>(std::move(*this)); }
};

struct ForgetfulTile : Square {  // forgot both overrides
  int grout = kGrout;
};

}  // namespace shapes

constexpr auto kAccess = std::meta::access_context::unchecked();

consteval bool declares_override(std::meta::info cls, std::string_view hook) {
  for (auto member : std::meta::members_of(cls, kAccess)) {
    if (std::meta::is_function(member) && std::meta::has_identifier(member) &&
        std::meta::identifier_of(member) == hook && std::meta::is_override(member)) {
      return true;
    }
  }
  return false;
}

consteval std::vector<std::meta::info> concrete_figures(std::meta::info ns, std::meta::info base) {
  std::vector<std::meta::info> found;
  for (auto member : std::meta::members_of(ns, kAccess)) {
    if (std::meta::is_type(member) && std::meta::is_class_type(member) && member != base &&
        std::meta::is_base_of_type(base, member) && !std::meta::is_abstract_type(member)) {
      found.push_back(member);
    }
  }
  return found;
}

consteval std::vector<std::meta::info> lacking_hooks(std::meta::info ns, std::meta::info base) {
  std::vector<std::meta::info> found;
  for (auto cls : concrete_figures(ns, base)) {
    if (!declares_override(cls, "copy_") || !declares_override(cls, "move_")) found.push_back(cls);
  }
  return found;
}

constexpr auto kFigures = std::define_static_array(concrete_figures(^^shapes, ^^shapes::Figure));
constexpr auto kLacking = std::define_static_array(lacking_hooks(^^shapes, ^^shapes::Figure));

// What the lint must report: exactly ForgetfulTile.
static_assert(kLacking.size() == 1);
static_assert(kLacking[0] == ^^shapes::ForgetfulTile);
constexpr std::size_t kFigureCount = kFigures.size();
constexpr std::size_t kLackingCount = kLacking.size();

int main() {
  int unexpected = 0;

  std::println("lint: {} of {} concrete figures lack an override", kLackingCount, kFigureCount);
  template for (constexpr auto cls : kLacking) {
    std::println("  {} does not override copy_ and move_", std::meta::identifier_of(cls));
  }

  template for (constexpr auto cls : kFigures) {
    using T = typename [:cls:];
    T original{};
    const shapes::Figure& base = original;
    auto copy = base.clone();
    const shapes::Figure& copied = *copy;
    const bool keeps_type = typeid(copied) == typeid(original);
    constexpr bool expected = declares_override(cls, "copy_") && declares_override(cls, "move_");
    unexpected += keeps_type == expected ? 0 : 1;
    std::println("test: {} clone keeps its type: {}", std::meta::identifier_of(cls), keeps_type);
  }
  return unexpected;
}
