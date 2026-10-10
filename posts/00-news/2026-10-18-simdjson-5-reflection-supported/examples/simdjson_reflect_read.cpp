// simdjson_reflect_read.cpp
// simdjson 5.0 reads a struct through static reflection. The JSON keys are in a
// different order than the struct members; the reflective read uses key
// selectors by default (release notes for 5.0.0, 28 Sep 2026). The program also
// asks for the number type of 2^64 - 1 and of 2^64: since 5.0 an integer in
// [2^64, 10^20) is reported as a big integer.
//
// Expected output (exit code 0):
//   name=Ada age=42 active=true
//   home=Wroclaw 50001
//   tags=2 cpp,json
//   18446744073709551615 -> uint64
//   18446744073709551616 -> big_integer
//
// Compile: g++ -std=c++26 -freflection -O2 simdjson_reflect_read.cpp simdjson.cpp
//          (Compiler Explorer: g162 with the simdjson 5.0.2 library, version id 502)
// verify: ce-only
#include <iostream>
#include <string>
#include <vector>
#include "simdjson.h"

using namespace simdjson;

struct Address {
    std::string city;
    int zip;
};

struct User {
    std::string name;
    int age;
    bool active;
    Address home;
    std::vector<std::string> tags;
};

int main() {
    // The keys are NOT in declaration order.
    auto json = R"({
        "tags": ["cpp", "json"],
        "active": true,
        "home": { "zip": 50001, "city": "Wroclaw" },
        "age": 42,
        "name": "Ada"
    })"_padded;

    ondemand::parser parser;
    ondemand::document doc = parser.iterate(json);
    User u;
    if (auto err = doc.get(u)) {
        std::cout << "error: " << error_message(err) << '\n';
        return 1;
    }
    std::cout << "name=" << u.name << " age=" << u.age
              << " active=" << std::boolalpha << u.active << '\n';
    std::cout << "home=" << u.home.city << ' ' << u.home.zip << '\n';
    std::cout << "tags=" << u.tags.size() << ' ' << u.tags[0] << ',' << u.tags[1] << '\n';

    // An integer of exactly 2^64 is a big integer in 5.0.
    auto big = R"([18446744073709551615, 18446744073709551616])"_padded;
    ondemand::document bdoc = parser.iterate(big);
    for (ondemand::value v : bdoc.get_array()) {
        auto t = v.get_number_type().value();
        std::cout << v.raw_json_token() << " -> "
                  << (t == ondemand::number_type::big_integer ? "big_integer" : "uint64")
                  << '\n';
    }
    return 0;
}
