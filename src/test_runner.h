#pragma once

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <limits>
#include <optional>

namespace TestRunnerPrivate {
  template <
    class Map
  >
  std::ostream& PrintMap(std::ostream& os, const Map& m) {
    os << "{";
    bool first = true;
    for (const auto& kv : m) {
      if (!first) {
        os << ", ";
      }
      first = false;
      os << kv.first << ": " << kv.second;
    }
    return os << "}";
  }
}

template <class T>
std::ostream& operator << (std::ostream& os, const std::optional<T>& opt) {
  if (opt)
    os << *opt;
  else
    os << "empty opt";
  return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class T>
std::ostream& operator << (std::ostream& os, const std::set<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class K, class V>
std::ostream& operator << (std::ostream& os, const std::map<K, V>& m) {
  return TestRunnerPrivate::PrintMap(os, m);
}

template <class K, class V>
std::ostream& operator << (std::ostream& os, const std::unordered_map<K, V>& m) {
  return TestRunnerPrivate::PrintMap(os, m);
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {}) {
  if (!(t == u)) {
    std::ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
       os << " hint: " << hint;
    }
    throw std::runtime_error(os.str());
  }
}

template <class T>
bool operator==(const std::vector<T>& v1, const std::vector<T>& v2) {
  if (v1.size() != v2.size())
    return false;
  size_t sz = v1.size();
  for (size_t i = 0; i < sz; ++i)
    if (!(v1[i] == v2[i]))
      return false;
  return true;
}

template<typename T, typename U>
void AssertClose(const T& t, const U& u, const std::string& hint = {}) {
  if (!(std::abs(t - u) <= std::numeric_limits<double>::epsilon()*0.5*std::abs(t+u))) {
    std::ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
       os << " hint: " << hint;
    }
    throw std::runtime_error(os.str());
  }
}

inline void Assert(bool b, const std::string& hint) {
  AssertEqual(b, true, hint);
}

class TestRunner {
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const std::string& test_name) {
    try {
      func();
      std::cerr << test_name << " OK" << std::endl;
    } catch (std::exception& e) {
      ++fail_count;
      std::cerr << test_name << " fail: " << e.what() << std::endl;
    } catch (...) {
      ++fail_count;
      std::cerr << "Unknown exception caught" << std::endl;
    }
  }

  ~TestRunner() {
    std::cerr.flush();
    if (fail_count > 0) {
      std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
      exit(1);
    }
  }

private:
  int fail_count = 0;
};

#ifndef FILE_NAME
#define FILE_NAME __FILE__
#endif

#define ASSERT_EQUAL(x, y) {                          \
  std::ostringstream __assert_equal_private_os;       \
  __assert_equal_private_os                           \
    << #x << " != " << #y << ", "                     \
    << FILE_NAME << ":" << __LINE__;                  \
  AssertEqual(x, y, __assert_equal_private_os.str()); \
}

#define ASSERT_CLOSE(x, y) {                          \
  std::ostringstream __assert_equal_private_os;       \
  __assert_equal_private_os                           \
    << #x << " != " << #y << ", "                     \
    << FILE_NAME << ":" << __LINE__;                  \
  AssertClose(x, y, __assert_equal_private_os.str()); \
}

#define ASSERT(x) {                           \
  std::ostringstream __assert_private_os;     \
  __assert_private_os << #x << " is false, "  \
    << FILE_NAME << ":" << __LINE__;          \
  Assert(static_cast<bool>(x), __assert_private_os.str());       \
}

#define RUN_TEST(tr, func) \
  tr.RunTest(func, #func)

#define ASSERT_THROWS(expr, expected_exception) {                                           \
  bool __assert_private_flag = true;                                                        \
  try {                                                                                     \
    expr;                                                                                   \
    __assert_private_flag = false;                                                          \
  } catch (expected_exception&) {                                                           \
  } catch (...) {                                                                           \
    std::ostringstream __assert_private_os;                                                 \
    __assert_private_os << "Expression " #expr " threw an unexpected exception"             \
      " " FILE_NAME ":" << __LINE__;                                                        \
    Assert(false, __assert_private_os.str());                                               \
  }                                                                                         \
  if (!__assert_private_flag){                                                              \
    std::ostringstream __assert_private_os;                                                 \
    __assert_private_os << "Expression " #expr " is expected to throw " #expected_exception \
      " " FILE_NAME ":" << __LINE__;                                                        \
    Assert(false, __assert_private_os.str());                                               \
  }                                                                                         \
}

#define ASSERT_DOESNT_THROW(expr)                                           \
  try {                                                                     \
    expr;                                                                   \
  } catch (...) {                                                           \
    std::ostringstream __assert_private_os;                                 \
    __assert_private_os << "Expression " #expr " threw an unexpected exception" \
      " " FILE_NAME ":" << __LINE__;                                        \
    Assert(false, __assert_private_os.str());                               \
  }
