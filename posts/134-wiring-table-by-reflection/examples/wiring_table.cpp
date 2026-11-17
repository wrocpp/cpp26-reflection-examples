// verify: gcc-only
// A signal-to-slot wiring table in two forms, over a small signal type of this file (NOT Qt: Qt's connect() was not run).
//  1. ProcedureTuple keeps a static constexpr tuple of pairs of member pointers and connects it with std::apply and a
//     comma fold, the design of Leonardo Rossi's LinkedIn post.
//  2. Procedure puts the table on the slots: [[=on{^^Step::failed}]] on the function that handles it, read back by
//     reflection. A static_assert over the table checks every edge.
// Build: g++ -std=c++26 -freflection -Wall -Wextra   (Compiler Explorer: g162)
// The program has no macros and exits 0. The cases that must not compile are quoted in the README.
// Limits: one class, one translation unit; signals are data members of type Signal<Args...> and a slot must take exactly
// the signal's argument types (Qt also accepts fewer); an annotation is checked only where something walks it.
#include <meta>

#include <algorithm>
#include <any>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <ranges>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

namespace wiring {

// ---- the signal type: a vector of connections -----------------------------------------------------------------
template <class... Args>
class Signal {
 public:
  template <class Receiver>
  void connect(Receiver* receiver, void (Receiver::*slot)(Args...)) {
    connections_.push_back({.receiver = receiver,
                            .slot = slot,
                            .call = [receiver, slot](Args... args) { (receiver->*slot)(args...); }});
  }

  template <class Receiver>
  void disconnect(const Receiver* receiver, void (Receiver::*slot)(Args...)) {
    std::erase_if(connections_, [&](const Connection& c) { return is(c, receiver, slot); });
  }

  void emit(Args... args) const {
    for (const auto& c : connections_) c.call(args...);
  }

  [[nodiscard]] std::size_t size() const { return connections_.size(); }

 private:
  struct Connection {
    const void* receiver;
    std::any slot;
    std::function<void(Args...)> call;
  };

  template <class Receiver>
  static bool is(const Connection& c, const Receiver* receiver, void (Receiver::*slot)(Args...)) {
    const auto* stored = std::any_cast<void (Receiver::*)(Args...)>(&c.slot);
    return c.receiver == receiver && stored != nullptr && *stored == slot;
  }

  std::vector<Connection> connections_;
};

struct Step {
  Signal<int> failed;
  Signal<int> timedOut;
  Signal<> succeeded;
};

constexpr int kFailureCode = 7;
constexpr int kTimeoutSeconds = 30;

// ---- form 1: the tuple of member pointers, plain C++17 ----------------------------------------------------------
struct ProcedureTuple {
  int failures{};
  int timeouts{};
  int successes{};

  void onStepFailed(int) { ++failures; }
  void onStepTimedOut(int) { ++timeouts; }
  void onStepSucceeded() { ++successes; }

  static constexpr auto wiring() {
    return std::make_tuple(std::pair{&Step::failed, &ProcedureTuple::onStepFailed},
                           std::pair{&Step::timedOut, &ProcedureTuple::onStepTimedOut},
                           std::pair{&Step::succeeded, &ProcedureTuple::onStepSucceeded});
  }

  template <bool Enable>
  void toggleWiring(Step& step) {
    const auto toggle = [&](const auto& pair) {
      auto& signal = step.*pair.first;
      if constexpr (Enable) {
        signal.connect(this, pair.second);
      } else {
        signal.disconnect(this, pair.second);
      }
    };
    std::apply([&](const auto&... pairs) { (toggle(pairs), ...); }, wiring());
  }
};

// ---- form 2: the table sits on the slots ------------------------------------------------------------------------
struct on {
  std::meta::info signal;
};

struct Edge {
  std::meta::info signal;
  std::meta::info slot;
};

// The one place that looks at annotations. It loops over annotations_of and compares the type, without const, with ^^on, because the
// clang fork has no annotations_of_with_type.
consteval std::vector<Edge> edges_of(std::meta::info cls) {
  std::vector<Edge> out;
  for (const auto member : std::meta::members_of(cls, std::meta::access_context::current())) {
    for (const auto annotation : std::meta::annotations_of(member)) {
      if (std::meta::remove_const(std::meta::type_of(annotation)) == ^^on) {
        out.push_back({.signal = std::meta::extract<on>(annotation).signal, .slot = member});
      }
    }
  }
  return out;
}

template <class Owner>
inline constexpr auto table_of = std::define_static_array(edges_of(^^Owner));

consteval bool names_a_signal_of_step(std::meta::info signal) {
  return std::meta::is_nonstatic_data_member(signal) && std::meta::parent_of(signal) == ^^Step;
}

consteval bool signature_matches(Edge edge) {
  const auto args = std::meta::template_arguments_of(std::meta::type_of(edge.signal));
  const auto params = std::meta::parameters_of(edge.slot);
  return std::ranges::equal(args, params, [](auto arg, auto param) {
    return std::meta::dealias(arg) == std::meta::dealias(std::meta::type_of(param));
  });
}

consteval bool has_no_repeated_edge(std::span<const Edge> table) {
  return std::ranges::none_of(std::views::iota(std::size_t{0}, table.size()), [&](std::size_t i) {
    return std::ranges::any_of(table | std::views::drop(i + 1), [&](Edge e) {
      return e.signal == table[i].signal && e.slot == table[i].slot;
    });
  });
}

template <class Owner>
consteval bool table_is_valid() {
  constexpr auto table = table_of<Owner>;
  return has_no_repeated_edge(table) && std::ranges::all_of(table, [](Edge e) {
    return names_a_signal_of_step(e.signal) && signature_matches(e);
  });
}

template <bool Enable, class Owner>
void toggleWiring(Step& step, Owner& owner) {
  static_assert(table_is_valid<Owner>(),
                "a wiring annotation names something that is not a signal of Step, repeats an edge, or its slot "
                "takes other argument types than the signal");
  template for (constexpr Edge edge : table_of<Owner>) {
    if constexpr (Enable) {
      step.[:edge.signal:].connect(&owner, &[:edge.slot:]);
    } else {
      step.[:edge.signal:].disconnect(&owner, &[:edge.slot:]);
    }
  }
}

struct Procedure {
  int failures{};
  int timeouts{};
  int successes{};

  [[=on{^^Step::failed}]] void onStepFailed(int) { ++failures; }
  [[=on{^^Step::timedOut}]] void onStepTimedOut(int) { ++timeouts; }
  [[=on{^^Step::succeeded}]] void onStepSucceeded() { ++successes; }
  void helper() {}   // no annotation: not wired
};

// One signal on two slots is two connections, and so is a slot on two signals.
struct Fanout {
  int calls{};

  [[=on{^^Step::failed}, =on{^^Step::timedOut}]] void onEither(int) { ++calls; }
  [[=on{^^Step::failed}]] void onFailed(int) { ++calls; }
};

constexpr std::size_t kProcedureEdges = 3;
constexpr std::size_t kFanoutEdges = 3;
static_assert(table_of<Procedure>.size() == kProcedureEdges);
static_assert(table_of<Fanout>.size() == kFanoutEdges);
static_assert(table_is_valid<Procedure>());
static_assert(table_is_valid<Fanout>());

struct Counts {
  int failures;
  int timeouts;
  int successes;
  bool operator==(const Counts&) const = default;
};

template <class Owner>
Counts counts(const Owner& owner) {
  return {.failures = owner.failures, .timeouts = owner.timeouts, .successes = owner.successes};
}

void fire_all(const Step& step) {
  step.failed.emit(kFailureCode);
  step.timedOut.emit(kTimeoutSeconds);
  step.succeeded.emit();
}

}  // namespace wiring

int main() {
  using namespace wiring;
  constexpr Counts kOnce{.failures = 1, .timeouts = 1, .successes = 1};

  Step step;
  ProcedureTuple tuple_form;
  Procedure annotated_form;
  Fanout fanout;

  fire_all(step);
  std::printf("before connect: tuple=%d annotated=%d\n", tuple_form.failures, annotated_form.failures);

  tuple_form.toggleWiring<true>(step);
  toggleWiring<true>(step, annotated_form);
  toggleWiring<true>(step, fanout);
  std::printf("connections: failed=%zu timedOut=%zu succeeded=%zu\n", step.failed.size(), step.timedOut.size(),
              step.succeeded.size());
  fire_all(step);
  std::printf("after connect: tuple=%d/%d/%d annotated=%d/%d/%d fanout=%d\n", tuple_form.failures,
              tuple_form.timeouts, tuple_form.successes, annotated_form.failures, annotated_form.timeouts,
              annotated_form.successes, fanout.calls);
  const bool connected_ok = counts(tuple_form) == kOnce && counts(annotated_form) == kOnce;
  const int fanout_after_connect = fanout.calls;

  tuple_form.toggleWiring<false>(step);
  toggleWiring<false>(step, annotated_form);
  toggleWiring<false>(step, fanout);
  fire_all(step);
  std::printf("after disconnect: connections=%zu tuple=%d/%d/%d annotated=%d/%d/%d fanout=%d\n",
              step.failed.size() + step.timedOut.size() + step.succeeded.size(), tuple_form.failures,
              tuple_form.timeouts, tuple_form.successes, annotated_form.failures, annotated_form.timeouts,
              annotated_form.successes, fanout.calls);
  const bool disconnected_ok = counts(tuple_form) == kOnce && counts(annotated_form) == kOnce &&
                               fanout.calls == fanout_after_connect;
  return connected_ok && disconnected_ok ? 0 : 1;
}
