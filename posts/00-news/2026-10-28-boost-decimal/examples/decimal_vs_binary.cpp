// decimal_vs_binary.cpp
//
// Why a decimal floating point type exists, shown with the arithmetic that
// motivates it.
//
// Boost.Decimal (Matt Borland and Chris Kormanyos) was accepted after two
// Boost reviews and first shipped in Boost 1.91.0. It implements IEEE 754
// decimal floating point: decimal32_t, decimal64_t, decimal128_t.
//
// Compiler Explorer's Boost tops out at 1.90.0, so the library half of the
// story cannot be run here. This file runs the half that needs no library:
// what binary floating point does with values that are exact in decimal and
// not in binary.
//
// Plain portable C++: no library, no reflection, same output on GCC and clang.
//
// Compile: g++ -std=c++23 decimal_vs_binary.cpp

#include <cmath>
#include <cstdio>

int main()
{
    // ---------------------------------------------------------------------
    // 1. The library's own motivating example: add 0.1 a thousand times.
    //    In decimal that is exactly 100. In binary 0.1 is not representable,
    //    so the error accumulates.
    // ---------------------------------------------------------------------
    double d = 0.0;
    for (int i = 0; i < 1000; ++i)
        d += 0.1;

    std::printf("0.1 added 1000 times\n");
    std::printf("  double : %.17g\n", d);
    std::printf("  exact  : %.17g\n", 100.0);
    std::printf("  equal  : %s\n", (d == 100.0) ? "yes" : "no");
    std::printf("  off by : %.3g\n", d - 100.0);

    // ---------------------------------------------------------------------
    // 2. The same shape in money. A hundred one-cent items should total
    //    exactly one unit of currency.
    // ---------------------------------------------------------------------
    double cents = 0.0;
    for (int i = 0; i < 100; ++i)
        cents += 0.01;

    std::printf("\n0.01 added 100 times\n");
    std::printf("  double : %.17g\n", cents);
    std::printf("  equal to 1.00 : %s\n", (cents == 1.00) ? "yes" : "no");

    // ---------------------------------------------------------------------
    // 3. Storing integer cents fixes the total and does not fix rounding.
    //    A price of 1.005 should round up to 1.01 at two decimal places.
    //    The nearest double to 1.005 sits just below it, so it rounds down.
    // ---------------------------------------------------------------------
    long long cents_exact = 0;
    for (int i = 0; i < 100; ++i)
        cents_exact += 1;

    std::printf("\ninteger cents: %lld  (exact)\n", cents_exact);

    const double price = 1.005;
    const double rounded = std::round(price * 100.0) / 100.0;
    std::printf("price 1.005 stored as double : %.17g\n", price);
    std::printf("  rounded to 2 places        : %.2f\n", rounded);
    std::printf("  rounded up to 1.01         : %s\n",
                (rounded == 1.01) ? "yes" : "no");

    // ---------------------------------------------------------------------
    // 4. The single value at the root of it. 0.1 has no finite binary
    //    expansion, so the nearest double is slightly off.
    // ---------------------------------------------------------------------
    std::printf("\nnearest double to 0.1 : %.20f\n", 0.1);
    std::printf("nearest double to 0.5 : %.20f  (exact, it is a power of two)\n", 0.5);

    return 0;
}
