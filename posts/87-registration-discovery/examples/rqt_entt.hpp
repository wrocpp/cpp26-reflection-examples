// rqt::register_type<T>(ctx): register a type with entt::meta by walking its reflection.
// Produces the same registrations as the hand-written entt::meta_factory chain.
#pragma once
#include <meta>
#include <vector>
#include <entt/entt.hpp>

namespace rqt {

namespace detail {

consteval bool is_reflectable_function(std::meta::info m) {
    return std::meta::is_function(m) && std::meta::has_identifier(m) &&
           !std::meta::is_special_member_function(m) && !std::meta::is_static_member(m);
}

template <std::meta::access_context Ctx>
consteval std::vector<std::meta::info> member_functions_of(std::meta::info type) {
    std::vector<std::meta::info> out;
    for (std::meta::info m : std::meta::members_of(type, Ctx)) {
        if (is_reflectable_function(m)) out.push_back(m);
    }
    return out;
}

}  // namespace detail

// Access: current() (default) only sees what the caller of register_type may name;
// pass std::meta::access_context::unchecked() to see private members (they then fail at the splice).
template <class T, std::meta::access_context Ctx = std::meta::access_context::current()>
void register_type(entt::meta_ctx& ctx) {
    auto f = entt::meta_factory<T>{ctx}.type(std::define_static_string(std::meta::identifier_of(^^T)));
    template for (constexpr auto b : std::define_static_array(std::meta::bases_of(^^T, Ctx))) {
        f.template base<typename [:std::meta::type_of(b):]>();
    }
    template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, Ctx))) {
        f.template data<&[:m:]>(std::define_static_string(std::meta::identifier_of(m)));
    }
    template for (constexpr auto m : std::define_static_array(detail::member_functions_of<Ctx>(^^T))) {
        f.template func<&[:m:]>(std::define_static_string(std::meta::identifier_of(m)));
    }
}

// Enumerators become constant data (entt has no enum concept): data<E::red>("red").
template <class E>
    requires std::is_enum_v<E>
void register_enum(entt::meta_ctx& ctx) {
    auto f = entt::meta_factory<E>{ctx}.type(std::define_static_string(std::meta::identifier_of(^^E)));
    template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
        f.template data<([:e:])>(std::define_static_string(std::meta::identifier_of(e)));
    }
}

}  // namespace rqt
