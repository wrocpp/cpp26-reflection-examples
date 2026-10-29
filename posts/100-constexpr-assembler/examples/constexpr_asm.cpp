// verify: gcc-only
// A two-pass assembler that runs at compile time. Labels are the hard part of an assembler: a jump may name
// a label that is defined later, so the address is not known when the jump is read. The classic answer is
// two passes. Pass 1 walks the source and records where each label lands. Pass 2 emits the bytes, and every
// label is already known. In a constexpr function the two passes are just two loops, and the bytes are
// checked by static_assert before the program runs.
// The target is a handful of real x86-64 instructions. The program is then copied into executable memory
// and run, which works on x86-64 Linux only (Compiler Explorer g162); elsewhere the file still assembles
// and checks the bytes.
// Build: g++-16 -std=c++23 -Wall -Wextra -Werror constexpr_asm.cpp   (Compiler Explorer: g162)
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string_view>

#if defined(__x86_64__) && defined(__linux__)
#include <cstring>
#include <sys/mman.h>
#endif

namespace casm {

using Byte = std::uint8_t;

// Encodings, named once. REX.W selects 64-bit operands. ModRM mod=11 means "register direct", the
// register fields are shifted into place, and the group-1 extensions pick add, sub or cmp.
constexpr Byte kNop = 0x90;
constexpr Byte kRet = 0xC3;
constexpr Byte kRexW = 0x48;
constexpr Byte kMovImm32 = 0xC7;       // /0: mov r/m64, imm32
constexpr Byte kAddRegReg = 0x01;      // add r/m64, r64
constexpr Byte kGroup1Imm32 = 0x81;    // /0 add, /5 sub, /7 cmp: r/m64, imm32
constexpr Byte kJmpRel32 = 0xE9;
constexpr Byte kTwoByte = 0x0F;        // escape for the two-byte opcodes
constexpr Byte kJneRel32 = 0x85;
constexpr Byte kModRegDirect = 0xC0;
constexpr int kRegFieldShift = 3;
constexpr int kExtAdd = 0, kExtSub = 5, kExtCmp = 7;
constexpr int kRax = 0, kRcx = 1;

constexpr std::size_t kMaxLabels = 8;
constexpr std::size_t kImm32Bytes = 4;

// Not defined anywhere: calling it in a constant expression is a compile error, which is how an
// assembler written as a constexpr function reports a bad program.
[[noreturn]] void fail(const char* message);

// The views below point into the program text. The program is a constexpr string literal, which has static
// storage duration, so it outlives every Line, Label and Layout made from it.
struct Line {
  std::string_view label;
  std::string_view mnemonic;
  std::string_view op1;
  std::string_view op2;
};

constexpr std::string_view trim(std::string_view s) {
  while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
  while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.remove_suffix(1);
  return s;
}

constexpr Line parse_line(std::string_view text) {
  Line line{};
  if (const auto comment = text.find(';'); comment != std::string_view::npos) text = text.substr(0, comment);
  text = trim(text);
  if (const auto colon = text.find(':'); colon != std::string_view::npos) {
    line.label = trim(text.substr(0, colon));
    text = trim(text.substr(colon + 1));
  }
  if (text.empty()) return line;
  const auto space = text.find(' ');
  line.mnemonic = text.substr(0, space);
  if (space == std::string_view::npos) return line;
  text = trim(text.substr(space + 1));
  const auto comma = text.find(',');
  line.op1 = trim(text.substr(0, comma));
  if (comma != std::string_view::npos) line.op2 = trim(text.substr(comma + 1));
  return line;
}

constexpr int reg(std::string_view name) {
  if (name == "rax") return kRax;
  if (name == "rcx") return kRcx;
  fail("unknown register");
}

constexpr bool is_reg(std::string_view s) { return s == "rax" || s == "rcx"; }

constexpr std::int32_t imm(std::string_view s) {
  const bool negative = !s.empty() && s.front() == '-';
  if (negative) s.remove_prefix(1);
  if (s.empty()) fail("missing immediate");
  std::int64_t value = 0;
  for (const char c : s) {
    if (c < '0' || c > '9') fail("bad immediate");
    value = value * 10 + (c - '0');
  }
  return static_cast<std::int32_t>(negative ? -value : value);
}

constexpr std::size_t size_of(const Line& line) {
  const auto& m = line.mnemonic;
  if (m.empty()) return 0;
  if (m == "nop" || m == "ret") return 1;
  if (m == "add" && is_reg(line.op2)) return 3;
  if (m == "mov" || m == "add" || m == "sub" || m == "cmp") return 7;
  if (m == "jmp") return 5;
  if (m == "jne") return 6;
  fail("unknown instruction");
}

struct Label {
  std::string_view name;   // a view into the program text, see Line
  std::size_t offset = 0;
};

struct Layout {
  std::array<Label, kMaxLabels> labels{};
  std::size_t count = 0;
  std::size_t size = 0;
};

template <class F>
constexpr void for_each_line(std::string_view source, F&& f) {
  while (!source.empty()) {
    const auto end = source.find('\n');
    f(parse_line(source.substr(0, end)));
    if (end == std::string_view::npos) break;
    source.remove_prefix(end + 1);
  }
}

// Pass 1: where does each label land, and how long is the program?
constexpr Layout layout(std::string_view source) {
  Layout out;
  for_each_line(source, [&](const Line& line) {
    if (!line.label.empty()) {
      if (out.count == kMaxLabels) fail("too many labels");
      out.labels[out.count++] = Label{line.label, out.size};
    }
    out.size += size_of(line);
  });
  return out;
}

constexpr std::size_t address_of(const Layout& l, std::string_view name) {
  for (std::size_t i = 0; i < l.count; ++i)
    if (l.labels[i].name == name) return l.labels[i].offset;
  fail("undefined label");
}

// Pass 2: emit bytes. Every label is known, including the ones defined after the jump that names them.
template <std::size_t N>
constexpr std::array<Byte, N> assemble(std::string_view source) {
  const Layout l = layout(source);
  if (l.size != N) fail("size does not match the layout");
  std::array<Byte, N> code{};
  std::size_t at = 0;
  const auto put = [&](Byte b) { code[at++] = b; };
  const auto put_imm32 = [&](std::int32_t v) {
    const auto u = static_cast<std::uint32_t>(v);
    for (std::size_t i = 0; i < kImm32Bytes; ++i) put(static_cast<Byte>(u >> (8 * i)));
  };
  const auto modrm = [](int ext_or_src, int dst) {
    return static_cast<Byte>(kModRegDirect | (ext_or_src << kRegFieldShift) | dst);
  };
  const auto rel32 = [&](const Line& line) {
    const auto end_of_instruction = static_cast<std::int64_t>(at + size_of(line));
    return static_cast<std::int32_t>(static_cast<std::int64_t>(address_of(l, line.op1)) - end_of_instruction);
  };

  for_each_line(source, [&](const Line& line) {
    const auto& m = line.mnemonic;
    if (m == "nop") { put(kNop); }
    else if (m == "ret") { put(kRet); }
    else if (m == "mov") { put(kRexW); put(kMovImm32); put(modrm(0, reg(line.op1))); put_imm32(imm(line.op2)); }
    else if (m == "add" && is_reg(line.op2)) { put(kRexW); put(kAddRegReg); put(modrm(reg(line.op2), reg(line.op1))); }
    else if (m == "add") { put(kRexW); put(kGroup1Imm32); put(modrm(kExtAdd, reg(line.op1))); put_imm32(imm(line.op2)); }
    else if (m == "sub") { put(kRexW); put(kGroup1Imm32); put(modrm(kExtSub, reg(line.op1))); put_imm32(imm(line.op2)); }
    else if (m == "cmp") { put(kRexW); put(kGroup1Imm32); put(modrm(kExtCmp, reg(line.op1))); put_imm32(imm(line.op2)); }
    else if (m == "jmp") { const auto d = rel32(line); put(kJmpRel32); put_imm32(d); }
    else if (m == "jne") { const auto d = rel32(line); put(kTwoByte); put(kJneRel32); put_imm32(d); }
  });
  return code;
}

constexpr std::size_t size_of_program(std::string_view source) { return layout(source).size; }

}  // namespace casm

// The program: sum 3 + 2 + 1. "jne top" is a backward reference. "jmp done" is a forward reference: the
// label is defined four lines later, which is the case one pass cannot handle.
constexpr std::string_view kProgram = R"(
        mov rax, 0
        mov rcx, 3
top:    add rax, rcx
        sub rcx, 1
        cmp rcx, 0
        jne top          ; backward: top is already known
        jmp done         ; forward: done is defined below
        nop              ; skipped by the jump
done:   ret
)";

constexpr auto kCode = casm::assemble<casm::size_of_program(kProgram)>(kProgram);

// The expected bytes, written out by hand from the encodings, not produced by the assembler.
constexpr std::array<casm::Byte, 44> kExpected = {
    0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,   //  0: mov rax, 0
    0x48, 0xC7, 0xC1, 0x03, 0x00, 0x00, 0x00,   //  7: mov rcx, 3
    0x48, 0x01, 0xC8,                           // 14: top: add rax, rcx
    0x48, 0x81, 0xE9, 0x01, 0x00, 0x00, 0x00,   // 17: sub rcx, 1
    0x48, 0x81, 0xF9, 0x00, 0x00, 0x00, 0x00,   // 24: cmp rcx, 0
    0x0F, 0x85, 0xE9, 0xFF, 0xFF, 0xFF,         // 31: jne top   (14 - 37 = -23)
    0xE9, 0x01, 0x00, 0x00, 0x00,               // 37: jmp done  (43 - 42 = +1)
    0x90,                                       // 42: nop
    0xC3,                                       // 43: done: ret
};
static_assert(kCode == kExpected);

// Flip this to 1 to see a bad program rejected while compiling. GCC 16.2 reports the line that failed:
//   error: call to non-'constexpr' function 'void casm::fail(const char*)'
//     fail("undefined label");
#if 0
constexpr std::string_view kBad = "        jmp nowhere\n";
constexpr auto kBadCode = casm::assemble<casm::size_of_program(kBad)>(kBad);
#endif

int main() {
  std::printf("%zu bytes:", kCode.size());
  for (const auto b : kCode) std::printf(" %02X", b);
  std::printf("\n");
#if defined(__x86_64__) && defined(__linux__)
  void* page = mmap(nullptr, kCode.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (page == MAP_FAILED) { std::puts("could not map executable memory"); return 1; }
  std::memcpy(page, kCode.data(), kCode.size());
  const auto run = reinterpret_cast<long (*)()>(page);
  std::printf("rax = %ld\n", run());
  munmap(page, kCode.size());
#else
  std::puts("not x86-64 Linux: assembled and checked, not run");
#endif
}
