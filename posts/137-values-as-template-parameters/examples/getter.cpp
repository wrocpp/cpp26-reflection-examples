// verify: gcc-only
// A member function pointer as a template argument against the same pointer as a run-time argument, for the assembly view.
// Build: g++ -std=c++26 -O2 -S -masm=intel   (Compiler Explorer: g162, assembly pane; also clang2210 and MSVC with /O2)
// The program exits 0. The README quotes the instructions of the two read functions on each compiler.
#include <cstdio>

constexpr double kIntensity = 2.5;

struct Sensor {
  double wavelength() const { return m_wavelength; }
  double intensity() const { return m_intensity; }
  double m_wavelength{};
  double m_intensity{};
};

template <auto Getter>
double readFrom(const Sensor& sensor) {
  return (sensor.*Getter)();
}

double viaTemplate(const Sensor& s) { return readFrom<&Sensor::intensity>(s); }
double viaPointer(const Sensor& s, double (Sensor::*getter)() const) { return (s.*getter)(); }

int main() {
  const Sensor sensor{.m_wavelength = 1.0, .m_intensity = kIntensity};
  std::printf("%g %g\n", viaTemplate(sensor), viaPointer(sensor, &Sensor::intensity));
  return 0;
}
