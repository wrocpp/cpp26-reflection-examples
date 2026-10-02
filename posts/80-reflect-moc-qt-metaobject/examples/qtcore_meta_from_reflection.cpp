// verify: ce-only
// Spike 06b: the Qt 6.10 meta-object generated from reflection, by a CRTP base.
//
//   struct Counter : rqt::QObjectBase<Counter> { ... annotated members ... };
//
// QObjectBase<D, B = QObject> provides staticMetaObject, metaObject(),
// qt_metacall, qt_metacast and qt_static_metacall. Every input moc would
// have parsed from the header comes from reflecting D: the string table,
// the method table (signals first, then slots, then invokables, each in
// declaration order), parameter types and NAMES, the property table and each
// property's NOTIFY signal index. The tables are fed to Qt's own
// QtMocHelpers::metaObjectData, so Qt lays out the uint data itself.
//
// Checks: string-based connect, invokeMethod with and without a return value,
// QMetaProperty read/write/notify, pointer-to-member connect, qobject_cast,
// and a QML engine binding to the property and handling the NOTIFY signal by
// parameter name.
//
// Run it in Docker with GCC 16.2 and Qt 6.10 (the image of the reflect-moc repository, docker/Dockerfile).
// On Compiler Explorer (g162, library qt 6.10.0, id 6100, -std=c++26 -freflection -fPIC) it compiles but
// does not link: that Qt6Core needs libpcre2-16.so.0, which the worker lacks.
//
// Qt's keyword macros `emit`, `signals` and `slots` expand to nothing or to
// access specifiers, which breaks any C++ name spelled that way (rqt::emit
// here). A reflection-based API must be compiled with QT_NO_KEYWORDS.
#define QT_NO_KEYWORDS
#include <meta>

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/qtmochelpers.h>

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdio>
#include <cstring>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace rqt {
namespace meta = std::meta;

struct signal_t {};
inline constexpr signal_t signal{};
struct slot_t {};
inline constexpr slot_t slot{};
struct invokable_t {};
inline constexpr invokable_t invokable{};
// On a data member. With notify, the NOTIFY signal is the member named "<name>Changed".
struct property {
  bool notify = false;
};

template <class Tag>
consteval bool has(meta::info r) {
  for (auto a : meta::annotations_of(r))
    if (meta::remove_cvref(meta::type_of(a)) == ^^Tag) return true;
  return false;
}

template <class Tag>
consteval Tag get(meta::info r) {
  for (auto a : meta::annotations_of(r))
    if (meta::remove_cvref(meta::type_of(a)) == ^^Tag) return meta::extract<Tag>(a);
  throw meta::exception("missing annotation", r);
}

// --- reflection tables -----------------------------------------------------------

// Qt's method order: every signal, then slots, then invokables.
consteval std::vector<meta::info> methods_of(meta::info cls) {
  std::vector<meta::info> signals_, slots_, invokables;
  for (auto m : meta::members_of(cls, meta::access_context::unchecked())) {
    if (!meta::is_function(m)) continue;
    if (has<signal_t>(m))
      signals_.push_back(m);
    else if (has<slot_t>(m))
      slots_.push_back(m);
    else if (has<invokable_t>(m))
      invokables.push_back(m);
  }
  signals_.insert(signals_.end(), slots_.begin(), slots_.end());
  signals_.insert(signals_.end(), invokables.begin(), invokables.end());
  return signals_;
}

consteval std::size_t signal_count(meta::info cls) {
  std::size_t n = 0;
  for (auto m : methods_of(cls)) n += has<signal_t>(m);
  return n;
}

consteval std::vector<meta::info> properties_of(meta::info cls) {
  std::vector<meta::info> out;
  for (auto m : meta::nonstatic_data_members_of(cls, meta::access_context::unchecked()))
    if (has<property>(m)) out.push_back(m);
  return out;
}

consteval std::string_view param_name(meta::info p) {
  return meta::has_identifier(p) ? meta::identifier_of(p) : std::string_view{};
}

// The string table: class name first (Qt requires index 0), then "", then
// every method, parameter and property name, deduplicated.
consteval std::vector<std::string_view> strings_of(meta::info cls) {
  std::vector<std::string_view> out{meta::identifier_of(cls), ""};
  auto add = [&out](std::string_view s) {
    if (std::ranges::find(out, s) == out.end()) out.push_back(s);
  };
  for (auto m : methods_of(cls)) {
    add(meta::identifier_of(m));
    for (auto p : meta::parameters_of(m)) add(param_name(p));
  }
  for (auto p : properties_of(cls)) add(meta::identifier_of(p));
  return out;
}

consteval uint string_index(meta::info cls, std::string_view s) {
  auto all = strings_of(cls);
  return static_cast<uint>(std::ranges::find(all, s) - all.begin());
}

consteval meta::info notify_signal(meta::info cls, meta::info prop) {
  std::string name{meta::identifier_of(prop)};
  name += "Changed";
  for (auto m : methods_of(cls))
    if (has<signal_t>(m) && meta::identifier_of(m) == name) return m;
  throw meta::exception("property has notify=true but no signal named " + name, prop);
}

consteval uint notify_index(meta::info cls, meta::info prop) {
  if (!get<property>(prop).notify) return uint(-1);
  auto sig = notify_signal(cls, prop);
  auto all = methods_of(cls);
  return static_cast<uint>(std::ranges::find(all, sig) - all.begin());
}

consteval std::span<char const* const> string_list(meta::info cls) {
  std::vector<char const*> v;
  for (auto s : strings_of(cls)) v.push_back(std::define_static_string(s));
  return std::define_static_array(v);
}

// Duck-types QtMocHelpers::StringRefStorage: metaObjectData only needs
// StringCount, StringSize and writeTo. Qt's own type wants char arrays with
// a known extent, which define_static_string pointers do not have.
template <class D>
struct string_table {
  static constexpr std::span<char const* const> list = string_list(^^D);
  static constexpr int StringCount = static_cast<int>(list.size());
  static constexpr std::size_t StringSize = [] {
    std::size_t n = 0;
    for (auto s : list) n += std::string_view{s}.size() + 1;
    return n;
  }();

  constexpr void writeTo(uint (&offsets)[2 * StringCount], char (&data)[StringSize]) const {
    uint offset = 0;
    for (int i = 0; i < StringCount; ++i) {
      std::string_view s{list[i]};
      for (std::size_t j = 0; j <= s.size(); ++j) data[offset + j] = list[i][j];
      offsets[2 * i] = offset + sizeof(offsets);
      offsets[2 * i + 1] = static_cast<uint>(s.size());
      offset += static_cast<uint>(s.size() + 1);
    }
  }
};

template <class T>
consteval uint type_id() {
  using U = std::remove_cvref_t<T>;
  if constexpr (std::is_void_v<U>)
    return QMetaType::Void;
  else if constexpr (QMetaTypeId2<U>::IsBuiltIn)
    return QMetaTypeId2<U>::MetaType;
  else
    static_assert(false, "spike: only built-in meta types; others need IsUnresolvedType | name index");
}

consteval uint access_of(meta::info m) {
  namespace QMC = QtMocConstants;
  return meta::is_private(m) ? QMC::AccessPrivate : meta::is_protected(m) ? QMC::AccessProtected : QMC::AccessPublic;
}

// Each FunctionData specialization nests its own FunctionParameter, so
// SignalData<F>::ParametersArray and MethodData<F>::ParametersArray are
// distinct types: build the array for the Data type actually constructed.
template <class Data, class D, meta::info M>
constexpr Data function_data() {
  using R = [:meta::return_type_of(M):];
  using Params = typename Data::ParametersArray;
  constexpr Params params = []<std::size_t... P>(std::index_sequence<P...>) {
    return Params{{{type_id<typename[:meta::type_of(meta::parameters_of(M)[P]):]>(),
                    string_index(^^D, param_name(meta::parameters_of(M)[P]))}...}};
  }(std::make_index_sequence<meta::parameters_of(M).size()>{});
  return Data(string_index(^^D, meta::identifier_of(M)), string_index(^^D, ""), access_of(M), type_id<R>(), params);
}

template <class D, std::size_t I>
constexpr auto method_data() {
  constexpr meta::info m = methods_of(^^D)[I];
  using F = [:meta::type_of(m):];
  if constexpr (has<signal_t>(m))
    return function_data<QtMocHelpers::SignalData<F>, D, m>();
  else if constexpr (has<slot_t>(m))
    return function_data<QtMocHelpers::SlotData<F>, D, m>();
  else
    return function_data<QtMocHelpers::MethodData<F>, D, m>();
}

template <class D, std::size_t I>
constexpr auto property_data() {
  constexpr meta::info p = properties_of(^^D)[I];
  using T = [:meta::type_of(p):];
  namespace QMC = QtMocConstants;
  return QtMocHelpers::PropertyData<T>(string_index(^^D, meta::identifier_of(p)), type_id<T>(),
                                       QMC::DefaultPropertyFlags | QMC::Writable, notify_index(^^D, p));
}

template <class D>
struct meta_tag {};

template <class D>
constexpr auto meta_content = [] {
  auto methods = []<std::size_t... I>(std::index_sequence<I...>) {
    return QtMocHelpers::UintData{method_data<D, I>()...};
  }(std::make_index_sequence<methods_of(^^D).size()>{});
  auto properties = []<std::size_t... I>(std::index_sequence<I...>) {
    return QtMocHelpers::UintData{property_data<D, I>()...};
  }(std::make_index_sequence<properties_of(^^D).size()>{});
  QtMocHelpers::UintData enums{};
  return QtMocHelpers::metaObjectData<D, meta_tag<D>>(QtMocConstants::MetaObjectFlag{}, string_table<D>{}, methods,
                                                      properties, enums);
}();

// --- dispatch ----------------------------------------------------------------------

// qt_metacall style: args[0] is the return slot, args[1..] point to the params.
template <meta::info Fn, class C>
void call_with_args(C* self, void** args) {
  [&]<std::size_t... I>(std::index_sequence<I...>) {
    using R = [:meta::return_type_of(Fn):];
    if constexpr (std::is_void_v<R>) {
      self->[:Fn:](*static_cast<std::remove_cvref_t<typename[:meta::type_of(meta::parameters_of(Fn)[I]):]>*>(
          args[I + 1])...);
    } else {
      R r = self->[:Fn:](*static_cast<std::remove_cvref_t<typename[:meta::type_of(meta::parameters_of(Fn)[I]):]>*>(
          args[I + 1])...);
      if (args[0]) *static_cast<R*>(args[0]) = std::move(r);
    }
  }(std::make_index_sequence<meta::parameters_of(Fn).size()>{});
}

template <meta::info P, class D>
void write_property(D* t, void* v) {
  using T = [:meta::type_of(P):];
  auto& value = *static_cast<T*>(v);
  if (t->[:P:] == value) return;
  t->[:P:] = value;
  if constexpr (get<property>(P).notify) {
    constexpr auto sig = notify_signal(^^D, P);
    if constexpr (meta::parameters_of(sig).size() == 0)
      t->[:sig:]();
    else
      t->[:sig:](t->[:P:]);
  }
}

template <class D>
void static_metacall(D* t, QMetaObject::Call c, int id, void** a) {
  constexpr std::size_t methods = methods_of(^^D).size();
  constexpr std::size_t props = properties_of(^^D).size();
  constexpr std::size_t signals_ = signal_count(^^D);
  if (c == QMetaObject::InvokeMetaMethod) {
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      ((id == int(I) ? (call_with_args<methods_of(^^D)[I]>(t, a), true) : false) || ...);
    }(std::make_index_sequence<methods>{});
  }
  if (c == QMetaObject::IndexOfMethod) {
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      (QtMocHelpers::indexOfMethod<decltype(&[:methods_of(^^D)[I]:])>(a, &[:methods_of(^^D)[I]:], int(I)) || ...);
    }(std::make_index_sequence<signals_>{});
  }
  if (c == QMetaObject::ReadProperty) {
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      ((id == int(I) ? (*static_cast<typename[:meta::type_of(properties_of(^^D)[I]):]*>(a[0]) =
                            t->[:properties_of(^^D)[I]:],
                        true)
                     : false) ||
       ...);
    }(std::make_index_sequence<props>{});
  }
  if (c == QMetaObject::WriteProperty) {
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      ((id == int(I) ? (write_property<properties_of(^^D)[I]>(t, a[0]), true) : false) || ...);
    }(std::make_index_sequence<props>{});
  }
}

// --- the CRTP base -------------------------------------------------------------------
struct object_tag {};

template <class D, class B = QObject>
struct QObjectBase : B, object_tag {
  using B::B;

  static const QMetaObject staticMetaObject;

  const QMetaObject* metaObject() const override {
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
  }

  void* qt_metacast(const char* name) override {
    if (!name) return nullptr;
    if (!std::strcmp(name, string_table<D>::list[0])) return static_cast<void*>(static_cast<D*>(this));
    return B::qt_metacast(name);
  }

  int qt_metacall(QMetaObject::Call c, int id, void** a) override {
    constexpr int methods = static_cast<int>(methods_of(^^D).size());
    constexpr int props = static_cast<int>(properties_of(^^D).size());
    id = B::qt_metacall(c, id, a);
    if (id < 0) return id;
    if (c == QMetaObject::InvokeMetaMethod) {
      if (id < methods) qt_static_metacall(this, c, id, a);
      id -= methods;
    }
    if (c == QMetaObject::RegisterMethodArgumentMetaType) {
      if (id < methods) *static_cast<QMetaType*>(a[0]) = QMetaType();
      id -= methods;
    }
    if (c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty || c == QMetaObject::ResetProperty ||
        c == QMetaObject::BindableProperty || c == QMetaObject::RegisterPropertyMetaType) {
      qt_static_metacall(this, c, id, a);
      id -= props;
    }
    return id;
  }

  static void qt_static_metacall(QObject* o, QMetaObject::Call c, int id, void** a) {
    static_metacall(static_cast<D*>(o), c, id, a);
  }
};

template <class D, class B>
const QMetaObject QObjectBase<D, B>::staticMetaObject = {
    {QMetaObject::SuperData::link<B::staticMetaObject>(), meta_content<D>.staticData.stringdata,
     meta_content<D>.staticData.data, qt_static_metacall, nullptr, meta_content<D>.relocatingData.metaTypes,
     nullptr}};

// --- emit: form (a) from spike 05 ------------------------------------------------------
consteval int signal_index(meta::info f) {
  if (meta::is_function(f) && meta::is_class_member(f) && has<signal_t>(f)) {
    auto all = methods_of(meta::parent_of(f));
    return static_cast<int>(std::ranges::find(all, f) - all.begin());
  }
  throw meta::exception("rqt::emit used outside a [[=rqt::signal]] member function", f);
}

struct signal_id {
  int index;
  consteval signal_id(meta::info caller = meta::current_function()) : index(signal_index(caller)) {}
};

template <class C>
struct emit {
  C* self;
  signal_id id;
  emit(C* s, signal_id i = {}) : self(s), id(i) {}
  template <class... A>
  void operator()(A const&... a) const {
    void* args[] = {nullptr, const_cast<void*>(static_cast<void const*>(&a))...};
    QMetaObject::activate(self, &C::staticMetaObject, id.index, args);
  }
};
}  // namespace rqt

#ifndef SPIKE_NO_QOBJECT_SPECIALIZATION
// qobject_cast and the pointer-to-member connect static_assert
// HasQ_OBJECT_Macro<D>, which requires &D::qt_metacall to be a member of D
// itself. A CRTP base's override is a member of the base, so tell Qt directly.
namespace QtPrivate {
template <class D>
  requires std::derived_from<D, rqt::object_tag>
struct HasQ_OBJECT_Macro<D> {
  enum { Value = 1 };
};
}  // namespace QtPrivate
#endif

// --- the user class: no Q_OBJECT, no moc -------------------------------------------------
struct Counter : rqt::QObjectBase<Counter> {
  [[= rqt::property{.notify = true}]] int value = 0;

  [[= rqt::signal]] void valueChanged(int value) { rqt::emit{this}(value); }

  [[= rqt::slot]] void reset() { setValue(0); }
  [[= rqt::slot]] void setValue(int v) {
    if (v == value) return;
    value = v;
    valueChanged(v);
  }

  [[= rqt::invokable]] int add(int n) {
    setValue(value + n);
    return value;
  }

  void not_exported() {}
};

static void check_core() {
  Counter a, b;

  bool ok = QObject::connect(&a, SIGNAL(valueChanged(int)), &b, SLOT(setValue(int)));
  assert(ok);
  a.setValue(5);
  assert(b.value == 5);

  ok = QMetaObject::invokeMethod(&a, "setValue", Q_ARG(int, 7));
  assert(ok && a.value == 7 && b.value == 7);
  int sum = 0;
  ok = QMetaObject::invokeMethod(&a, "add", Q_RETURN_ARG(int, sum), Q_ARG(int, 3));
  assert(ok && sum == 10);
  ok = QMetaObject::invokeMethod(&a, "reset");
  assert(ok && a.value == 0);

  QMetaObject const* mo = a.metaObject();
  assert(mo == &Counter::staticMetaObject);
  assert(std::strcmp(mo->className(), "Counter") == 0);
  assert(mo->superClass() == &QObject::staticMetaObject);
  assert(mo->methodCount() - mo->methodOffset() == 4);
  for (int i = mo->methodOffset(); i < mo->methodCount(); ++i) {
    QMetaMethod m = mo->method(i);
    std::printf("  method %d: %s type=%d params=%s\n", i - mo->methodOffset(), m.methodSignature().constData(),
                int(m.methodType()), m.parameterNames().join(',').constData());
  }
  assert(mo->method(mo->methodOffset()).methodType() == QMetaMethod::Signal);
  assert(mo->method(mo->methodOffset()).parameterNames().value(0) == "value");

  QMetaProperty p = mo->property(mo->indexOfProperty("value"));
  assert(p.isValid() && p.isReadable() && p.isWritable());
  assert(p.write(&a, 12) && a.value == 12 && b.value == 12);
  assert(p.read(&a).toInt() == 12);
  assert(p.hasNotifySignal() && p.notifySignal().name() == "valueChanged");
  assert(a.property("value").toInt() == 12);

  int seen = 0;
  QObject::connect(&a, &Counter::valueChanged, &a, [&](int v) { seen = v; });
  a.setValue(20);
  assert(seen == 20);

  QObject* o = &a;
  QObject plain;
  assert(qobject_cast<Counter*>(o) == &a);
  assert(qobject_cast<Counter*>(&plain) == nullptr);
  assert(a.inherits("QObject") && a.inherits("Counter"));
}

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  check_core();
  std::puts("06b reflected meta-object: SIGNAL/SLOT connect, invokeMethod, QMetaProperty, PMF connect, qobject_cast OK");
}
