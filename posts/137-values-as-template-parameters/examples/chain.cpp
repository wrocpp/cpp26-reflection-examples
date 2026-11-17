// verify: gcc-only
// The C++17 way: a chain of if constexpr branches that maps each enumerator to its SQL name, with a static_assert in the last
// branch so a new enumerator stops the build. Every enumerator appears twice, in the enum and in the chain.
// Build: g++ -std=c++17   (Compiler Explorer: g162 and clang2210, no reflection needed)
// The program exits 0. The README quotes the message a fourth enumerator produces.
#include <cstdio>
#include <string_view>

enum class Column { Name, Wavelength, Intensity };

template <Column Col>
constexpr std::string_view sqlName() {
  if constexpr (Col == Column::Name) {
    return "name";
  } else if constexpr (Col == Column::Wavelength) {
    return "wavelength_nm";
  } else {
    static_assert(Col == Column::Intensity, "Map the new Column here.");
    return "intensity";
  }
}

static_assert(sqlName<Column::Name>() == "name");
static_assert(sqlName<Column::Intensity>() == "intensity");

int main() {
  std::printf("%s %s %s\n", sqlName<Column::Name>().data(), sqlName<Column::Wavelength>().data(),
              sqlName<Column::Intensity>().data());
  return 0;
}
