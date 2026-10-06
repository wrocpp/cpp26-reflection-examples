// verify: gcc-only
// GENERATED single file: reflect-moc core (include/reflect_moc/core/*.hpp at commit f6827b1 of the local public clone)
// inlined into examples/core/worker_demo.cpp by examples/core/amalgamate.py. Nothing here was edited by hand.
// Build: g++-16 -std=c++26 -freflection -pthread worker_demo_single_file.cpp   (Compiler Explorer: g162)
// A Worker on a background thread reports progress to a Ui on the main
// thread, with no moc and no Qt: C++26 reflection finds the signals.
//
//   Ui::startRequested ---queued--> Worker::start          (runs on the worker)
//   Worker.progress (NOTIFY) ---queued--> Ui::onProgress   (runs on main)
//   Worker::finished ---queued--> Ui::onFinished           (runs on main)
//
// Build: g++-16 -std=c++26 -freflection -pthread -Iinclude examples/core/worker_demo.cpp
// Compiler Explorer (GCC 16.2, g162): paste the generated single file
// <build>/examples/core/worker_demo_single_file.cpp, flags -std=c++26 -freflection -pthread.

// reflect-moc standalone core: signals, slots, properties and thread
// affinity for classes derived from rqt::Object, driven by C++26 reflection.


namespace rqt {

// Marks a member function as a signal (form a). Its body is the one line
// `rqt::emit{this}(args...);`.
struct signal_t {};
inline constexpr signal_t signal{};

// Marks a member function as a slot. Optional: any member function or
// callable can be connected; the annotation is for introspection only.
struct slot_t {};
inline constexpr slot_t slot{};

// Marks a data member as a property, accessed with `obj.set<"name">(v)` and
// `obj.get<"name">()`. With `.notify = true` the class gets a generated
// signal `<name>Changed(T const&)` that `set` emits when the value changes.
struct property {
  bool notify = false;
};

// A static descriptor signal (form b): `static constexpr rqt::signal_of<int> valueChanged{};`
template <class... A>
struct signal_of;

}  // namespace rqt


#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <utility>

#if !defined(__cpp_lib_move_only_function)
#error "reflect_moc core needs std::move_only_function (__cpp_lib_move_only_function)"
#endif

namespace rqt {

// A unit of work posted to a thread's event loop.
using task = std::move_only_function<void()>;

namespace detail {

// The queue of tasks posted to one thread. It exists before that thread
// starts its EventLoop, so a post never races the loop's construction.
class mailbox {
 public:
  void push(task t) {
    {
      std::scoped_lock lock{mutex_};
      tasks_.push_back(std::move(t));
    }
    ready_.notify_one();
  }

  // Everything queued so far, without waiting.
  std::deque<task> take() {
    std::scoped_lock lock{mutex_};
    return std::exchange(tasks_, {});
  }

  // Waits for tasks or a quit request; nullopt means quit.
  std::optional<std::deque<task>> wait() {
    std::unique_lock lock{mutex_};
    ready_.wait(lock, [this] { return quitting_ || !tasks_.empty(); });
    if (std::exchange(quitting_, false)) return std::nullopt;
    return std::exchange(tasks_, {});
  }

  void quit() {
    {
      std::scoped_lock lock{mutex_};
      quitting_ = true;
    }
    ready_.notify_one();
  }

 private:
  std::mutex mutex_;
  std::condition_variable ready_;
  std::deque<task> tasks_;
  bool quitting_ = false;
};

struct mailbox_registry {
  std::mutex mutex;
  std::unordered_map<std::thread::id, std::shared_ptr<mailbox>> boxes;
};

inline mailbox_registry& registry() {
  static mailbox_registry instance;
  return instance;
}

// The mailbox of `thread`, created on first use.
inline std::shared_ptr<mailbox> mailbox_for(std::thread::id thread) {
  auto& r = registry();
  std::scoped_lock lock{r.mutex};
  auto& box = r.boxes[thread];
  if (!box) box = std::make_shared<mailbox>();
  return box;
}

inline void retire_mailbox(std::thread::id thread) {
  auto& r = registry();
  std::scoped_lock lock{r.mutex};
  r.boxes.erase(thread);
}

// Runs the batch in order, destroying each task before the next starts, so a
// blocking emitter waiting on a task wakes as soon as its task is done.
inline void run_all(std::deque<task>& batch) {
  while (!batch.empty()) {
    task next = std::move(batch.front());
    batch.pop_front();
    next();
  }
}

}  // namespace detail

// The event loop of the thread that constructs it; one per thread.
// Queued signal deliveries and `rqt::post` calls for that thread run inside
// `run()` or `process_events()`, in the order they were posted.
class EventLoop {
 public:
  // Throws std::logic_error if this thread already has an EventLoop.
  EventLoop() : thread_{std::this_thread::get_id()}, mailbox_{detail::mailbox_for(thread_)} {
    if (current()) throw std::logic_error("rqt::EventLoop: this thread already has an event loop");
    set_current(this);
  }

  // Tasks still queued for this thread are destroyed without running.
  ~EventLoop() {
    set_current(nullptr);
    detail::retire_mailbox(thread_);
  }

  EventLoop(EventLoop const&) = delete;
  EventLoop& operator=(EventLoop const&) = delete;

  // Runs tasks until quit() is called. Tasks posted after quit() stay queued
  // for the next run() or process_events().
  void run() {
    while (auto batch = mailbox_->wait()) detail::run_all(*batch);
  }

  // Runs the tasks queued so far and returns how many there were, including
  // queued deliveries dropped because their receiver died.
  std::size_t process_events() {
    auto batch = mailbox_->take();
    std::size_t const count = batch.size();
    detail::run_all(batch);
    return count;
  }

  // Thread-safe: makes run() return after the task it is running.
  void quit() { mailbox_->quit(); }

  std::thread::id thread() const noexcept { return thread_; }

  // The loop of the calling thread, or nullptr.
  static EventLoop* current() noexcept { return current_slot(); }

 private:
  static EventLoop*& current_slot() noexcept {
    thread_local EventLoop* loop = nullptr;
    return loop;
  }
  static void set_current(EventLoop* loop) noexcept { current_slot() = loop; }

  std::thread::id thread_;
  std::shared_ptr<detail::mailbox> mailbox_;
};

// Queues `t` to run on `thread`'s event loop. A thread that has no loop yet
// keeps the task until its loop starts.
inline void post(std::thread::id thread, task t) { detail::mailbox_for(thread)->push(std::move(t)); }

}  // namespace rqt

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <latch>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <span>
#include <thread>
#include <utility>
#include <vector>

namespace rqt {

// How a signal reaches a slot.
enum class connection_type {
  automatic,       // direct if the receiver lives on the emitting thread, else queued
  direct,          // called on the emitting thread, arguments by reference
  queued,          // posted to the receiver's thread, arguments copied
  blocking_queued  // queued, and the emitter waits for the slot; direct on the receiver's own thread
};

namespace detail {

// Pointers to the emitted arguments, borrowed for the duration of the emit.
using arg_span = std::span<void const* const>;

struct connection_record;

// The part of an Object that outlives it for as long as a connection, a
// queued event or an emit in progress still refers to it.
struct object_state {
  explicit object_state(std::thread::id home) : affinity{home} {}

  std::atomic<std::thread::id> affinity;
  std::atomic<bool> alive{true};
  std::mutex mutex;                                          // guards outgoing
  std::vector<std::shared_ptr<connection_record>> outgoing;  // connections from this sender
};

// Who a connection delivers to. An unbound connection (a free callable with
// no context object) is always called directly.
struct receiver_token {
  std::weak_ptr<object_state> state;
  bool bound;
};

// The receiver's thread, or nullopt once it is destroyed.
inline std::optional<std::thread::id> home_of(receiver_token const& receiver) {
  if (!receiver.bound) return std::this_thread::get_id();
  auto const state = receiver.state.lock();
  if (!state || !state->alive.load()) return std::nullopt;
  return state->affinity.load();
}

// A type-erased slot: the callable and two thunks generated for the signal's
// parameter types.
struct slot_call {
  std::shared_ptr<void const> target;
  void (*invoke)(void const* target, arg_span args);
  task (*package)(std::shared_ptr<void const> const& target, arg_span args);
};

struct connection_spec {
  std::uint64_t id;
  void const* signal;
  receiver_token receiver;
  connection_type type;
  slot_call call;
};

struct connection_record {
  explicit connection_record(connection_spec s) : spec{std::move(s)} {}

  connection_spec const spec;
  std::atomic<bool> connected{true};
};

inline std::uint64_t next_connection_id() {
  static std::atomic<std::uint64_t> last{0};
  return last.fetch_add(1, std::memory_order_relaxed) + 1;
}

enum class delivery { direct, queued, blocking };

constexpr delivery delivery_for(connection_type type, bool receiver_is_here) {
  switch (type) {
    case connection_type::direct:
      return delivery::direct;
    case connection_type::queued:
      return delivery::queued;
    case connection_type::blocking_queued:
      return receiver_is_here ? delivery::direct : delivery::blocking;
    case connection_type::automatic:
      break;
  }
  return receiver_is_here ? delivery::direct : delivery::queued;
}

// A queued delivery. The arguments are copied here: this is the hand-off to
// another thread. The receiver's liveness is checked again on its own thread.
inline task queued_call(connection_record const& c, arg_span args) {
  return [receiver = c.spec.receiver, call = c.spec.call.package(c.spec.call.target, args)]() mutable {
    if (home_of(receiver)) call();
  };
}

// Counts the latch down when the task that owns it is destroyed, whether it
// ran or was dropped, so a blocking emitter never waits forever.
class completion {
 public:
  explicit completion(std::shared_ptr<std::latch> done) : done_{std::move(done)} {}
  completion(completion&&) noexcept = default;
  completion& operator=(completion&&) noexcept = default;
  ~completion() {
    if (done_) done_->count_down();
  }

 private:
  std::shared_ptr<std::latch> done_;
};

inline void post_and_wait(std::thread::id home, task call) {
  auto done = std::make_shared<std::latch>(1);
  post(home, [signal_done = completion{done}, call = std::move(call)]() mutable { call(); });
  done->wait();
}

inline void deliver(connection_record const& c, arg_span args) {
  auto const home = home_of(c.spec.receiver);
  if (!home) return;
  switch (delivery_for(c.spec.type, *home == std::this_thread::get_id())) {
    case delivery::direct:
      return c.spec.call.invoke(c.spec.call.target.get(), args);
    case delivery::queued:
      return post(*home, queued_call(c, args));
    case delivery::blocking:
      return post_and_wait(*home, queued_call(c, args));
  }
}

// The connections of one signal, copied under the lock so that slots may
// connect and disconnect while the emit runs.
inline std::vector<std::shared_ptr<connection_record const>> snapshot(object_state& sender, void const* signal) {
  std::scoped_lock lock{sender.mutex};
  return sender.outgoing | std::views::filter([signal](auto const& c) { return c->spec.signal == signal; }) |
         std::ranges::to<std::vector<std::shared_ptr<connection_record const>>>();
}

inline void activate(object_state& sender, void const* signal, arg_span args) {
  for (auto const& c : snapshot(sender, signal))
    if (c->connected.load()) deliver(*c, args);
}

inline bool receiver_gone(std::shared_ptr<connection_record> const& c) { return !home_of(c->spec.receiver); }

inline std::shared_ptr<connection_record> attach(object_state& sender, connection_spec spec) {
  auto record = std::make_shared<connection_record>(std::move(spec));
  std::scoped_lock lock{sender.mutex};
  std::erase_if(sender.outgoing, receiver_gone);
  sender.outgoing.push_back(record);
  return record;
}

inline bool detach(object_state& sender, connection_record& record) {
  std::scoped_lock lock{sender.mutex};
  record.connected.store(false);
  return std::erase_if(sender.outgoing, [&record](auto const& c) { return c.get() == &record; }) > 0;
}

inline void detach_all(object_state& sender) {
  std::scoped_lock lock{sender.mutex};
  for (auto const& c : sender.outgoing) c->connected.store(false);
  sender.outgoing.clear();
}

inline std::size_t live_receivers(object_state& sender, void const* signal) {
  std::scoped_lock lock{sender.mutex};
  return static_cast<std::size_t>(std::ranges::count_if(
      sender.outgoing, [signal](auto const& c) { return c->spec.signal == signal && !receiver_gone(c); }));
}

}  // namespace detail

// A handle to one connection. Copyable; it does not keep the sender alive.
class connection {
 public:
  connection() = default;
  connection(std::weak_ptr<detail::object_state> sender, std::shared_ptr<detail::connection_record> const& record)
      : sender_{std::move(sender)}, record_{record}, id_{record->spec.id} {}

  // Unique for the life of the process; 0 for a default-constructed handle.
  std::uint64_t id() const noexcept { return id_; }

  // False once disconnected, or once the sender or the receiver is destroyed.
  bool connected() const {
    auto const record = record_.lock();
    return record && record->connected.load() && detail::home_of(record->spec.receiver);
  }

  // Safe to call from a slot while the signal is being emitted: the slot is
  // not called again, even later in the same emit. Returns false if the
  // connection was already gone.
  bool disconnect() {
    auto const sender = sender_.lock();
    auto const record = record_.lock();
    return sender && record && detail::detach(*sender, *record);
  }

 private:
  std::weak_ptr<detail::object_state> sender_;
  std::weak_ptr<detail::connection_record> record_;
  std::uint64_t id_ = 0;
};

}  // namespace rqt

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace rqt {

// A string literal usable as a template argument: `obj.set<"progress">(v)`.
template <std::size_t N>
struct fixed_string {
  char data[N]{};
  consteval fixed_string(char const (&s)[N]) { std::copy_n(s, N, data); }
  constexpr std::string_view view() const { return {data, N - 1}; }
};

}  // namespace rqt


#include <meta>

#include <algorithm>
#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace rqt {

// One row of a class's compile-time signal table.
struct signal_info {
  char const* name;
  std::size_t arity;
};

// Names a signal in `connect<...>`, `emit<...>` and `receivers<...>`: either
// by name, `"progressChanged"`, or by pointer, `&Button::clicked`,
// `&Slider::valueChanged` (a descriptor) or `&Worker::progress` (a NOTIFY
// property). Resolution happens at compile time; a name or pointer that is
// not a signal of the sender's class is a compile error.
struct signal_ref {
  char const* name;
  std::meta::info pointer;

  template <std::size_t N>
  consteval signal_ref(char const (&s)[N]) : name{std::define_static_string(std::string_view{s, N - 1})}, pointer{} {}

  template <class P>
    requires std::is_pointer_v<P> || std::is_member_pointer_v<P>
  consteval signal_ref(P p) : name{nullptr}, pointer{std::meta::reflect_constant(p)} {}
};

namespace detail {

namespace meta = std::meta;

inline constexpr std::string_view notify_suffix = "Changed";

template <class Tag>
consteval bool has_annotation(meta::info r) {
  return std::ranges::any_of(meta::annotations_of(r),
                             [](meta::info a) { return meta::remove_cvref(meta::type_of(a)) == ^^Tag; });
}

// Call only after has_annotation<Tag>(r).
template <class Tag>
consteval Tag annotation_of(meta::info r) {
  auto const all = meta::annotations_of(r);
  return meta::extract<Tag>(
      *std::ranges::find_if(all, [](meta::info a) { return meta::remove_cvref(meta::type_of(a)) == ^^Tag; }));
}

consteval bool is_function_signal(meta::info m) { return meta::is_function(m) && has_annotation<signal_t>(m); }

consteval bool is_descriptor_signal(meta::info m) {
  if (!meta::is_variable(m)) return false;
  auto const t = meta::remove_cvref(meta::type_of(m));
  return meta::has_template_arguments(t) && meta::template_of(t) == ^^signal_of;
}

consteval bool is_property(meta::info m) { return meta::is_nonstatic_data_member(m) && has_annotation<property>(m); }

consteval bool is_notify_property(meta::info m) { return is_property(m) && annotation_of<property>(m).notify; }

consteval bool is_signal(meta::info m) {
  return is_function_signal(m) || is_descriptor_signal(m) || is_notify_property(m);
}

consteval std::vector<meta::info> own_members_where(meta::info cls, bool (*pred)(meta::info)) {
  return meta::members_of(cls, meta::access_context::unchecked()) | std::views::filter(pred) |
         std::ranges::to<std::vector>();
}

// Members of `cls` and its bases that satisfy `pred`, bases first, each in
// declaration order.
consteval std::vector<meta::info> members_where(meta::info cls, bool (*pred)(meta::info)) {
  std::vector<meta::info> found;
  for (auto base : meta::bases_of(cls, meta::access_context::unchecked()))
    found.append_range(members_where(meta::type_of(base), pred));
  found.append_range(own_members_where(cls, pred));
  return found;
}

consteval std::vector<meta::info> signals_of(meta::info cls) { return members_where(cls, is_signal); }

consteval std::string signal_name(meta::info s) {
  std::string name{meta::identifier_of(s)};
  if (is_notify_property(s)) name += notify_suffix;
  return name;
}

consteval std::string class_name(meta::info cls) {
  return meta::has_identifier(cls) ? std::string{meta::identifier_of(cls)} : std::string{"this class"};
}

consteval std::string missing(std::string_view kind, std::string_view name, meta::info cls) {
  return std::string{"no "} + std::string{kind} + " named '" + std::string{name} + "' in " + class_name(cls);
}

consteval meta::info signal_named(meta::info cls, std::string_view name) {
  auto const all = signals_of(cls);
  auto const it = std::ranges::find_if(all, [name](meta::info s) { return signal_name(s) == name; });
  if (it == all.end()) throw meta::exception(missing("signal", name, cls), cls);
  return *it;
}

consteval meta::info property_named(meta::info cls, std::string_view name) {
  auto const all = members_where(cls, is_property);
  auto const it = std::ranges::find_if(all, [name](meta::info p) { return meta::identifier_of(p) == name; });
  if (it == all.end()) throw meta::exception(missing("property", name, cls), cls);
  return *it;
}

// True if the pointer P is &S (a member function, data member or descriptor).
template <meta::info S, auto P>
consteval bool designates() {
  if constexpr (std::is_same_v<decltype(&[:S:]), decltype(P)>)
    return &[:S:] == P;
  else
    return false;
}

template <class T, auto P>
consteval meta::info signal_at() {
  constexpr std::size_t count = signals_of(^^T).size();
  constexpr std::size_t hit = []<std::size_t... I>(std::index_sequence<I...>) {
    std::size_t found = count;
    (void)((designates<signals_of(^^T)[I], P>() ? (found = I, true) : false) || ...);
    return found;
  }(std::make_index_sequence<count>{});
  if (hit == count) throw meta::exception("the pointer does not name a signal of " + class_name(^^T), ^^T);
  return signals_of(^^T)[hit];
}

template <class T, signal_ref D>
consteval meta::info resolve_signal() {
  if constexpr (D.name != nullptr)
    return signal_named(^^T, D.name);
  else
    return signal_at<T, ([:D.pointer:])>();
}

consteval meta::info value_type_of(meta::info parameter) { return meta::remove_cvref(meta::type_of(parameter)); }

// The value types a signal carries, in order.
consteval std::vector<meta::info> signal_params(meta::info s) {
  if (is_descriptor_signal(s)) return meta::template_arguments_of(meta::remove_cvref(meta::type_of(s)));
  if (is_notify_property(s)) return {value_type_of(s)};
  return meta::parameters_of(s) | std::views::transform(value_type_of) | std::ranges::to<std::vector>();
}

// --- run-time identity of a signal --------------------------------------------
//
// A descriptor is its own key (its address). A function signal or a NOTIFY
// property is keyed by one byte of a per-class anchor array, at the signal's
// index among the class's own signals. The key cannot be a variable template
// specialised on the signal's reflection: see
// limit_current_function_reflection_as_template_argument.

consteval std::size_t own_signal_count(meta::info cls) { return own_members_where(cls, is_signal).size(); }

template <class C>
inline constexpr std::array<char, own_signal_count(^^C)> signal_anchor{};

template <class C>
inline constexpr char const* signal_anchor_begin = signal_anchor<C>.data();

consteval void const* anchored_key(meta::info s) {
  auto const cls = meta::parent_of(s);
  auto const own = own_members_where(cls, is_signal);
  auto const index = std::ranges::find(own, s) - own.begin();
  return meta::extract<char const*>(meta::substitute(^^signal_anchor_begin, {cls})) + index;
}

template <meta::info S>
consteval void const* signal_key() {
  if constexpr (is_descriptor_signal(S))
    return &[:S:];
  else
    return anchored_key(S);
}

// Per-type tags that let a form (a) emit check its arguments at run time.
template <class T>
inline constexpr char type_tag = 0;

template <class T>
inline constexpr void const* type_key = &type_tag<T>;

consteval void const* type_key_of(meta::info type) {
  return meta::extract<void const*>(meta::substitute(^^type_key, {type}));
}

consteval std::span<void const* const> param_type_keys(meta::info s) {
  return std::define_static_array(signal_params(s) | std::views::transform(type_key_of));
}

consteval signal_info describe(meta::info s) {
  return {.name = std::define_static_string(signal_name(s)), .arity = signal_params(s).size()};
}

}  // namespace detail

// The signals of T and its bases, bases first, in declaration order.
template <class T>
consteval std::span<signal_info const> signal_table() {
  return std::define_static_array(detail::signals_of(^^T) | std::views::transform(detail::describe));
}

}  // namespace rqt



#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace rqt::detail {

template <class F, class Params, std::size_t... I>
consteval bool invocable_with_prefix(std::index_sequence<I...>) {
  return std::is_invocable_v<F const&, std::tuple_element_t<I, Params> const&...>;
}

// A slot may take a prefix of the signal's arguments, as in Qt. The largest
// prefix it accepts wins; nullopt if it accepts none.
template <class F, class... P>
consteval std::optional<std::size_t> slot_arity() {
  using params = std::tuple<P...>;
  return []<std::size_t... K>(std::index_sequence<K...>) {
    std::optional<std::size_t> best;
    ((invocable_with_prefix<F, params>(std::make_index_sequence<K>{}) ? (best = K, true) : false), ...);
    return best;
  }(std::make_index_sequence<sizeof...(P) + 1>{});
}

template <class F, class Params, std::size_t... I>
void invoke_slot(F const& f, [[maybe_unused]] arg_span args, std::index_sequence<I...>) {
  f(*static_cast<std::tuple_element_t<I, Params> const*>(args[I])...);
}

// The copy into the task is the named ownership boundary: the arguments are
// handed to another thread and must outlive the emit.
template <class F, class Params, std::size_t... I>
task package_slot(std::shared_ptr<void const> const& target, [[maybe_unused]] arg_span args,
                  std::index_sequence<I...>) {
  return [target, copies = std::tuple<std::tuple_element_t<I, Params>...>{
                      *static_cast<std::tuple_element_t<I, Params> const*>(args[I])...}] {
    std::apply(*static_cast<F const*>(target.get()), copies);
  };
}

template <class F, class... P>
struct slot_thunks {
  using params = std::tuple<P...>;
  using prefix = std::make_index_sequence<*slot_arity<F, P...>()>;

  static void invoke(void const* target, arg_span args) {
    invoke_slot<F, params>(*static_cast<F const*>(target), args, prefix{});
  }
  static task package(std::shared_ptr<void const> const& target, arg_span args) {
    return package_slot<F, params>(target, args, prefix{});
  }
};

template <class F, class... P>
slot_call make_slot_call(F f) {
  static_assert(slot_arity<F, P...>().has_value(),
                "rqt::connect: the slot cannot be called with the signal's arguments (or a prefix of them)");
  return {.target = std::make_shared<F const>(std::move(f)),
          .invoke = &slot_thunks<F, P...>::invoke,
          .package = &slot_thunks<F, P...>::package};
}

// A member function slot bound to its receiver. Its liveness is guarded by
// the connection's receiver token, not by this pointer.
template <class Receiver, class Pmf>
struct member_slot {
  Receiver* receiver;
  Pmf pmf;

  template <class... A>
    requires std::is_invocable_v<Pmf const&, Receiver&, A const&...>
  void operator()(A const&... a) const {
    std::invoke(pmf, *receiver, a...);
  }
};

template <class Receiver, class Slot>
auto bind_receiver(Receiver& receiver, Slot slot) {
  if constexpr (std::is_member_function_pointer_v<Slot>)
    return member_slot<Receiver, Slot>{.receiver = &receiver, .pmf = slot};
  else
    return slot;
}

}  // namespace rqt::detail

#include <meta>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <span>
#include <thread>
#include <type_traits>
#include <utility>

namespace rqt {

class Object;

namespace detail {

struct access {
  static std::shared_ptr<object_state> const& handle(Object const& o) noexcept;
  static object_state& state(Object const& o) noexcept { return *handle(o); }
};

template <class... A>
std::array<void const*, sizeof...(A)> argument_pointers(A const&... args) {
  return {static_cast<void const*>(std::addressof(args))...};
}

// Direct calls borrow the arguments: `args` point at the caller's values, or
// at temporaries converted to the signal's parameter types.
template <class... P>
void emit_as(Object const& sender, void const* signal, std::type_identity_t<P> const&... args) {
  activate(access::state(sender), signal, argument_pointers(args...));
}

// Named function templates, not generic lambdas: GCC 16.2 does not treat a
// lambda body that splices signal_params(S)[I] as a constant context.
template <std::meta::info S, std::size_t... I>
constexpr auto emitter_for(std::index_sequence<I...>) {
  return &emit_as<typename[:signal_params(S)[I]:]...>;
}

template <std::meta::info S, class... A>
void emit_signal(Object const& sender, A const&... args) {
  static_assert(sizeof...(A) == signal_params(S).size(), "rqt::emit: wrong number of arguments for this signal");
  constexpr void const* key = signal_key<S>();
  constexpr auto emitter = emitter_for<S>(std::make_index_sequence<signal_params(S).size()>{});
  emitter(sender, key, args...);
}

template <std::meta::info S, class F, std::size_t... I>
slot_call bind_for_signal_impl(F f, std::index_sequence<I...>) {
  return make_slot_call<F, typename[:signal_params(S)[I]:]...>(std::move(f));
}

template <std::meta::info S, class F>
slot_call bind_for_signal(F f) {
  return bind_for_signal_impl<S>(std::move(f), std::make_index_sequence<signal_params(S).size()>{});
}

template <std::meta::info S, class F>
connection connect_to(Object const& sender, receiver_token receiver, connection_type type, F f) {
  constexpr void const* key = signal_key<S>();
  // Outside the braced list: see limit_info_template_id_in_designated_initializer.
  slot_call call = bind_for_signal<S>(std::move(f));
  auto const& handle = access::handle(sender);
  return connection{handle, attach(*handle, {.id = next_connection_id(),
                                             .signal = key,
                                             .receiver = std::move(receiver),
                                             .type = type,
                                             .call = std::move(call)})};
}

// Returns false, leaving `member` untouched, when the value is equal.
template <class T, class V>
bool assign_if_changed(T& member, V&& value) {
  if constexpr (std::equality_comparable_with<T const&, V const&>) {
    if (member == value) return false;
  }
  member = std::forward<V>(value);
  return true;
}

}  // namespace detail

// The base of every class with signals, slots or properties.
//
// Not a template, no virtual functions: its member templates take an
// explicit object parameter (`this Self& self`), so reflection sees the
// most-derived class the call is made on.
//
// Every Object has a thread affinity, initially the constructing thread.
// Destroying an Object disconnects everything it sends and everything it
// receives, and drops queued deliveries still addressed to it. Destroy an
// Object on its own thread.
class Object {
 public:
  Object() : state_{std::make_shared<detail::object_state>(std::this_thread::get_id())} {}
  Object(Object const&) = delete;
  Object& operator=(Object const&) = delete;
  ~Object() {
    state_->alive.store(false);
    detail::detach_all(*state_);
  }

  std::thread::id thread() const noexcept { return state_->affinity.load(); }

  // Deliveries resolved after this call target `target`; ones already queued
  // stay on the old thread's loop.
  void move_to_thread(std::thread::id target) noexcept { state_->affinity.store(target); }

  // Connects signal D of this object to `slot` on `receiver`: a member
  // function pointer of the receiver, or any callable, which then runs in the
  // receiver's thread and dies with it.
  template <signal_ref D, class Self, class Receiver, class Slot>
    requires std::derived_from<Receiver, Object>
  connection connect(this Self& self, Receiver& receiver, Slot slot,
                     connection_type type = connection_type::automatic) {
    constexpr std::meta::info S = detail::resolve_signal<Self, D>();
    return detail::connect_to<S>(self, {.state = detail::access::handle(receiver), .bound = true}, type,
                                 detail::bind_receiver(receiver, std::move(slot)));
  }

  // Connects signal D to a callable with no receiver; it is always called
  // directly on the emitting thread.
  template <signal_ref D, class Self, class F>
  connection connect(this Self& self, F f) {
    constexpr std::meta::info S = detail::resolve_signal<Self, D>();
    return detail::connect_to<S>(self, {.state = {}, .bound = false}, connection_type::direct, std::move(f));
  }

  // Emits signal D. The arguments convert to the signal's parameter types.
  template <signal_ref D, class Self, class... A>
  void emit(this Self const& self, A const&... args) {
    constexpr std::meta::info S = detail::resolve_signal<Self, D>();
    detail::emit_signal<S>(self, args...);
  }

  // How many connections of signal D have a live receiver.
  template <signal_ref D, class Self>
  std::size_t receivers(this Self const& self) {
    constexpr std::meta::info S = detail::resolve_signal<Self, D>();
    constexpr void const* key = detail::signal_key<S>();
    return detail::live_receivers(detail::access::state(self), key);
  }

  // Assigns the [[=rqt::property]] member N. For a NOTIFY property, emits
  // N##Changed with the new value, unless the value compares equal.
  template <fixed_string N, class Self, class V>
  void set(this Self& self, V&& value) {
    constexpr std::meta::info m = detail::property_named(^^Self, N.view());
    if (!detail::assign_if_changed(self.[:m:], std::forward<V>(value))) return;
    if constexpr (detail::is_notify_property(m)) detail::emit_signal<m>(self, self.[:m:]);
  }

  template <fixed_string N, class Self>
  auto const& get(this Self const& self) {
    constexpr std::meta::info m = detail::property_named(^^Self, N.view());
    return self.[:m:];
  }

 private:
  friend struct detail::access;
  std::shared_ptr<detail::object_state> state_;
};

namespace detail {

inline std::shared_ptr<object_state> const& access::handle(Object const& o) noexcept { return o.state_; }

consteval void const* function_signal_key(std::meta::info caller) {
  if (!is_function_signal(caller))
    throw std::meta::exception("rqt::emit{this} used outside a [[=rqt::signal]] member function", caller);
  return anchored_key(caller);
}

// The identity of the form (a) signal whose body constructs it, taken from
// std::meta::current_function() at the call site.
struct signal_id {
  void const* key;
  char const* name;
  // Static storage from define_static_array, so the view never dangles.
  std::span<void const* const> params;

  consteval signal_id(std::meta::info caller = std::meta::current_function())
      : key{function_signal_key(caller)},
        name{std::define_static_string(std::meta::identifier_of(caller))},
        params{param_type_keys(caller)} {}
};

template <class... A>
bool arguments_match(signal_id const& id) {
  std::array<void const*, sizeof...(A)> const given{type_key<std::remove_cvref_t<A>>...};
  return std::ranges::equal(id.params, given);
}

[[noreturn]] inline void emit_arguments_mismatch(char const* signal) {
  std::fprintf(stderr, "rqt::emit: the arguments do not match the parameters of signal '%s'\n", signal);
  std::abort();
}

}  // namespace detail

// Form (a): the body of a [[=rqt::signal]] member function,
//   [[=rqt::signal]] void clicked(int x) { rqt::emit{this}(x); }
// The arguments must have exactly the signal's parameter types (after
// remove_cvref). That is checked at run time, and a mismatch aborts: see
// limit_function_form_argument_types_checked_at_run_time.
template <class C>
struct emit {
  C* sender;
  detail::signal_id id;

  emit(C* s, detail::signal_id i = {}) : sender{s}, id{i} {}

  template <class... A>
  void operator()(A const&... args) const {
    static_assert(std::derived_from<std::remove_const_t<C>, Object>, "rqt::emit needs a class derived from rqt::Object");
    if (!detail::arguments_match<A...>(id)) detail::emit_arguments_mismatch(id.name);
    detail::activate(detail::access::state(*sender), id.key, detail::argument_pointers(args...));
  }
};

// Form (b): a static descriptor,
//   static constexpr rqt::signal_of<int> valueChanged{};
//   void setValue(int v) { valueChanged.emit(this, v); }
template <class... A>
struct signal_of {
  void emit(Object const* sender, std::type_identity_t<A> const&... args) const {
    detail::activate(detail::access::state(*sender), this, detail::argument_pointers(args...));
  }
};

// `rqt::connect<"progressChanged">(worker, ui, &Ui::onProgress)`
template <signal_ref D, class Sender, class... Rest>
connection connect(Sender& sender, Rest&&... rest) {
  return sender.template connect<D>(std::forward<Rest>(rest)...);
}

}  // namespace rqt

#include <print>
#include <string_view>
#include <thread>

namespace {

constexpr int not_started = -1;
constexpr int progress_step = 25;
constexpr int progress_done = 100;

std::thread::id main_thread;

std::string_view thread_label() { return std::this_thread::get_id() == main_thread ? "main" : "worker"; }

struct Worker : rqt::Object {
  [[= rqt::property{.notify = true}]] int progress = not_started;
  [[= rqt::signal]] void finished() { rqt::emit{this}(); }

  [[= rqt::slot]] void start() {
    std::println("start on {} thread", thread_label());
    for (int p = 0; p <= progress_done; p += progress_step) set<"progress">(p);
    finished();
  }
};

struct Ui : rqt::Object {
  [[= rqt::signal]] void startRequested() { rqt::emit{this}(); }

  [[= rqt::slot]] void onProgress(int value) { std::println("progress {} on {} thread", value, thread_label()); }
  [[= rqt::slot]] void onFinished() {
    std::println("finished on {} thread", thread_label());
    rqt::EventLoop::current()->quit();
  }
};

}  // namespace

int main() {
  main_thread = std::this_thread::get_id();
  rqt::EventLoop loop;
  Ui ui;
  Worker worker;
  std::jthread background{[] {
    rqt::EventLoop worker_loop;
    worker_loop.run();
  }};
  worker.move_to_thread(background.get_id());

  ui.connect<&Ui::startRequested>(worker, &Worker::start);
  rqt::connect<"progressChanged">(worker, ui, &Ui::onProgress);
  rqt::connect<"finished">(worker, ui, &Ui::onFinished);

  ui.startRequested();
  loop.run();
  rqt::post(background.get_id(), [] { rqt::EventLoop::current()->quit(); });
}
