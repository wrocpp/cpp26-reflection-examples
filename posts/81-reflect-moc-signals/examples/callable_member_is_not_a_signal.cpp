// verify: ce-only
// Qt's connect reads the class of a signal from QtPrivate::FunctionPointer, which exists for pointers to
// member functions. A callable data member has no such specialization, so neither form connects.
// Compiler Explorer: g162, library qt (id 6100), -std=c++26 -fPIC. Both errors are the expected result.
#include <QtCore/QObject>

#include <functional>

class WithFunctionMember : public QObject {
  Q_OBJECT
 public:
  std::function<void(int)> valueChanged;
};

class WithClosureMember : public QObject {
  Q_OBJECT
 public:
  static constexpr auto valueChanged = [](int) {};
};

void connect_function_member(WithFunctionMember& a, QObject& b) {
  QObject::connect(&a, &WithFunctionMember::valueChanged, &b, [](int) {});
}

void connect_closure_member(WithClosureMember& a, QObject& b) {
  QObject::connect(&a, &WithClosureMember::valueChanged, &b, [](int) {});
}
