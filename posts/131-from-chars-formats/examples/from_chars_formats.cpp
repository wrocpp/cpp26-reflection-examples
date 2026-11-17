// verify: gcc-only
// Build: g++ -std=c++26 -O2 from_chars_formats.cpp   (also builds with clang++ -std=c++26, libstdc++ or libc++)
// Prints one line per input: for each chars_format (g general, f fixed, s scientific, h hex)
// "chars consumed / errc / value after the call". The value starts as a sentinel, so a failed call shows -7.
// The program exits 0 only when every cell matches the table below: consumed count, errc, and, when
// the call succeeds, the exact value (a failed call must leave the sentinel untouched).
// Limits: on result_out_of_range the standard says the value is unmodified. libstdc++ does that;
// libc++ writes inf or 0. The table therefore does not assert the value of an out_of_range cell.
// Not tested: locale behaviour (from_chars has none by definition), float and long double.
#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>

namespace {

using std::chars_format;
using std::errc;

constexpr double kSentinel = -7.0;
constexpr std::size_t kFormatCount = 4;
constexpr std::size_t kBigDigits = 400;
constexpr std::size_t kLabelWidth = 8;
constexpr std::array<chars_format, kFormatCount> kFormats = {
    chars_format::general, chars_format::fixed, chars_format::scientific, chars_format::hex};
constexpr std::array<char, kFormatCount> kFormatLetters = {'g', 'f', 's', 'h'};

constexpr double kInf = std::numeric_limits<double>::infinity();
constexpr double kNan = std::numeric_limits<double>::quiet_NaN();

struct Expect {
  std::ptrdiff_t consumed;
  errc ec;
  double value;  // asserted only when ec is success
};

struct Row {
  std::string_view text;
  std::array<Expect, kFormatCount> cells;  // general, fixed, scientific, hex
};

constexpr errc kOk = errc{};
constexpr errc kBad = errc::invalid_argument;
constexpr errc kRange = errc::result_out_of_range;

constexpr Expect none{0, kBad, kSentinel};

constexpr auto kBigText = [] {
  std::array<char, kBigDigits> digits{};
  digits.fill('9');
  return digits;
}();

const std::array<Row, 14> kRows = {{
    {"1.5", {{{3, kOk, 1.5}, {3, kOk, 1.5}, none, {3, kOk, 1.3125}}}},
    {"1e3", {{{3, kOk, 1000.0}, {1, kOk, 1.0}, {3, kOk, 1000.0}, {3, kOk, 483.0}}}},
    {"1.5e", {{{3, kOk, 1.5}, {3, kOk, 1.5}, none, {4, kOk, 1.3671875}}}},
    {"0x1p4", {{{1, kOk, 0.0}, {1, kOk, 0.0}, none, {1, kOk, 0.0}}}},
    {"1p4", {{{1, kOk, 1.0}, {1, kOk, 1.0}, none, {3, kOk, 16.0}}}},
    {"+1", {{none, none, none, none}}},
    {" 1", {{none, none, none, none}}},
    {"inf", {{{3, kOk, kInf}, {3, kOk, kInf}, {3, kOk, kInf}, {3, kOk, kInf}}}},
    {"nan", {{{3, kOk, kNan}, {3, kOk, kNan}, {3, kOk, kNan}, {3, kOk, kNan}}}},
    {"1,5", {{{1, kOk, 1.0}, {1, kOk, 1.0}, none, {1, kOk, 1.0}}}},
    {"-0", {{{2, kOk, -0.0}, {2, kOk, -0.0}, none, {2, kOk, -0.0}}}},
    {"1e999", {{{5, kRange, 0.0}, {1, kOk, 1.0}, {5, kRange, 0.0}, {5, kOk, 125337.0}}}},
    {"1e-999", {{{6, kRange, 0.0}, {1, kOk, 1.0}, {6, kRange, 0.0}, {2, kOk, 30.0}}}},
    {std::string_view{kBigText.data(), kBigText.size()},
     {{{400, kRange, 0.0}, {400, kRange, 0.0}, none, {400, kRange, 0.0}}}},
}};

bool same_value(double got, double want) {
  if (std::isnan(want)) {
    return std::isnan(got);
  }
  return got == want && std::signbit(got) == std::signbit(want);
}

struct Observed {
  std::ptrdiff_t consumed;
  errc ec;
  double value;
};

Observed parse(std::string_view text, chars_format format) {
  double value = kSentinel;
  const auto result = std::from_chars(text.data(), text.data() + text.size(), value, format);
  return {result.ptr - text.data(), result.ec, value};
}

bool matches(const Observed& got, const Expect& want) {
  if (got.consumed != want.consumed || got.ec != want.ec) {
    return false;
  }
  if (want.ec == kOk) {
    return same_value(got.value, want.value);
  }
  if (want.ec == kBad) {
    return got.value == kSentinel;
  }
  return true;
}

const char* name_of(errc ec) {
  if (ec == kOk) {
    return "ok";
  }
  return ec == kBad ? "inv" : "rng";
}

std::string describe(const Row& row, const std::array<Observed, kFormatCount>& seen) {
  std::string line(row.text.substr(0, kLabelWidth));
  line += row.text.size() > kLabelWidth ? "..." : "";
  for (std::size_t i = 0; i < kFormatCount; ++i) {
    std::array<char, 96> cell{};
    std::snprintf(cell.data(), cell.size(), "  %c:%td/%s/%.17g", kFormatLetters[i],
                  seen[i].consumed, name_of(seen[i].ec), seen[i].value);
    line += cell.data();
  }
  return line;
}

bool check_row(const Row& row) {
  std::array<Observed, kFormatCount> seen{};
  bool ok = true;
  for (std::size_t i = 0; i < kFormatCount; ++i) {
    seen[i] = parse(row.text, kFormats[i]);
    ok = matches(seen[i], row.cells[i]) && ok;
  }
  std::printf("%s%s\n", describe(row, seen).c_str(), ok ? "" : "   <-- MISMATCH");
  return ok;
}

// strtod skips leading white space, takes a leading '+', and reads hex floats without being asked.
bool check_strtod_one(std::string_view text, double want) {
  char* end = nullptr;
  const double got = std::strtod(text.data(), &end);
  const std::ptrdiff_t consumed = end - text.data();
  std::printf("strtod \"%s\" -> %g, consumed %td\n", text.data(), got, consumed);
  return got == want && consumed == static_cast<std::ptrdiff_t>(text.size());
}

bool check_strtod() {
  const bool plus_ok = check_strtod_one(" +1", 1.0);
  const bool hex_ok = check_strtod_one("0x1p4", 16.0);
  return plus_ok && hex_ok;
}

}  // namespace

int main() {
  bool ok = true;
  for (const Row& row : kRows) {
    ok = check_row(row) && ok;
  }
  ok = check_strtod() && ok;
  return ok ? 0 : 1;
}
