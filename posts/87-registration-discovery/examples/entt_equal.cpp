// (A) hand-written vs reflection-driven entt::meta registration of the same struct.
#include <cstdio>
#include <cstdlib>
#include <string>
#include "rqt_entt.hpp"

struct Player {
    int hp{};
    float x{};
    float y{};
    std::string name;
    void heal(int n) { hp += n; }
};

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)

static void register_by_hand(entt::meta_ctx& ctx) {
    entt::meta_factory<Player>{ctx}
        .type("Player")
        .data<&Player::hp>("hp")
        .data<&Player::x>("x")
        .data<&Player::y>("y")
        .data<&Player::name>("name")
        .func<&Player::heal>("heal");
}

struct Summary {
    std::string type_name;
    entt::id_type type_id;
    std::size_t data_count, func_count;
    std::string data_names, func_names;
    int hp_after_set, hp_after_heal;
    std::string name_after_set;
    bool operator==(const Summary&) const = default;
};

static Summary probe(entt::meta_ctx& ctx) {
    auto t = entt::resolve<Player>(ctx);
    Summary s{std::string(t.name()), t.alias(), 0, 0, "", "", 0, 0, ""};
    for (auto&& [id, d] : t.data()) { ++s.data_count; s.data_names += std::string(d.name()) + ","; }
    for (auto&& [id, f] : t.func()) { ++s.func_count; s.func_names += std::string(f.name()) + ","; }
    Player p{10, 1.5f, 2.5f, "ann"};
    auto h = entt::meta_any{ctx, std::in_place_type<Player&>, p};
    CHECK(t.data(entt::hashed_string::value("hp")).set(h, 42));
    CHECK(t.data(entt::hashed_string::value("name")).set(h, std::string("bob")));
    s.hp_after_set = t.data(entt::hashed_string::value("hp")).get(h).cast<int>();
    s.name_after_set = t.data(entt::hashed_string::value("name")).get(h).cast<std::string>();
    CHECK(p.hp == 42 && p.name == "bob");
    CHECK(static_cast<bool>(t.invoke(entt::hashed_string::value("heal"), h, 8)));
    s.hp_after_heal = p.hp;
    return s;
}

int main() {
    entt::meta_ctx hand, refl;
    register_by_hand(hand);
    rqt::register_type<Player>(refl);
    auto a = probe(hand), b = probe(refl);
    std::printf("hand : %s id=%u data=%zu [%s] func=%zu [%s] hp %d -> %d name=%s\n", a.type_name.c_str(), a.type_id,
                a.data_count, a.data_names.c_str(), a.func_count, a.func_names.c_str(), a.hp_after_set, a.hp_after_heal,
                a.name_after_set.c_str());
    std::printf("refl : %s id=%u data=%zu [%s] func=%zu [%s] hp %d -> %d name=%s\n", b.type_name.c_str(), b.type_id,
                b.data_count, b.data_names.c_str(), b.func_count, b.func_names.c_str(), b.hp_after_set, b.hp_after_heal,
                b.name_after_set.c_str());
    CHECK(a == b);
    CHECK(a.data_count == 4 && a.func_count == 1 && a.hp_after_heal == 50);
    std::printf(failures == 0 ? "A: hand == reflected\n" : "A: MISMATCH\n");
    return failures == 0 ? 0 : 1;
}
