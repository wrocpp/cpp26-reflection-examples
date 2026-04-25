// Post 19 — reflect_llmschema: C++ struct → OpenAI/Anthropic tool-use JSON.
// Demonstrates the core walk — primitives, nested objects, enums.
// Compile: ./cpp posts/19-reflect-llmschema/examples/llmschema.cpp \
//                -o posts/19-reflect-llmschema/examples/llmschema
// Run:     ./run posts/19-reflect-llmschema/examples/llmschema

#include <experimental/meta>
#include <cstdint>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace llm {

struct description_tag {
    char const* text;
    bool operator==(description_tag const&) const = default;
};
consteval auto description(std::string_view s) -> description_tag {
    return {std::define_static_string(s)};
}

template <typename T> consteval void append_type_schema(std::string& out);

template <typename T>
consteval void append_object_schema(std::string& out) {
    out += R"({"type":"object","properties":{)";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ',';
        first = false;
        out += '"'; out += std::meta::identifier_of(m); out += "\":";
        using M = [: std::meta::type_of(m) :];
        append_type_schema<M>(out);
    }
    out += "},\"required\":[";
    first = true;
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ',';
        first = false;
        out += '"'; out += std::meta::identifier_of(m); out += '"';
    }
    out += "]}";
}

template <typename T>
consteval void append_type_schema(std::string& out) {
    if constexpr (std::is_same_v<T, bool>) {
        out += R"({"type":"boolean"})";
    } else if constexpr (std::is_integral_v<T>) {
        out += R"({"type":"integer"})";
    } else if constexpr (std::is_arithmetic_v<T>) {
        out += R"({"type":"number"})";
    } else if constexpr (std::is_convertible_v<T, std::string_view>) {
        out += R"({"type":"string"})";
    } else {
        append_object_schema<T>(out);
    }
}

template <typename ArgsStruct>
consteval char const* openai_tool_json(std::string_view name) {
    std::string out = R"({"type":"function","function":{"name":")";
    out += name;
    out += R"(",)";
    if constexpr (constexpr auto d
                  = std::meta::annotation_of_type<description_tag>(^^ArgsStruct)) {
        out += R"("description":")";
        out += d->text;
        out += R"(",)";
    }
    out += R"("parameters":)";
    append_object_schema<ArgsStruct>(out);
    out += "}}";
    return std::define_static_string(out);
}

}  // namespace llm

// --- user code --------------------------------------------------------

struct [[=llm::description("Fetch weather for a city.")]] WeatherArgs {
    std::string city;
    std::string country_code;
};

struct [[=llm::description("Create a new user account.")]] CreateUserArgs {
    std::string name;
    std::string email;
    bool admin;
    int age;
};

int main() {
    // Force consteval so the JSON goes into .rodata
    static constexpr char const* weather
        = llm::openai_tool_json<WeatherArgs>("get_weather");
    static constexpr char const* create
        = llm::openai_tool_json<CreateUserArgs>("create_user");
    std::println("{}", weather);
    std::println("{}", create);
}
