#pragma once

// Tiny dependency-free test harness (no network / package needed). Swap for doctest or Catch2
// whenever you like: the TEST_CASE / CHECK macros have the same shape.

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace testing {
    struct Case { std::string name; std::function<void()> run; };
    inline std::vector<Case>& Registry() { static std::vector<Case> r; return r; }
    inline int& Failures() { static int n = 0; return n; }
    struct Registrar { Registrar(const char* name, std::function<void()> fn) { Registry().push_back({name, std::move(fn)}); } };

    template <class T> std::string Show(const T& v) { std::ostringstream o; o << v; return o.str(); }
    inline std::string Show(bool v) { return v ? "true" : "false"; }
}

#define OB_CAT2(a, b) a##b
#define OB_CAT(a, b) OB_CAT2(a, b)

#define TEST_CASE(name)                                                                         \
    static void OB_CAT(ob_test_, __LINE__)();                                                   \
    static testing::Registrar OB_CAT(ob_reg_, __LINE__)(name, OB_CAT(ob_test_, __LINE__));      \
    static void OB_CAT(ob_test_, __LINE__)()

#define CHECK(expr)                                                                             \
    do { if (!(expr)) { ++testing::Failures();                                                  \
        std::cerr << "    FAIL " << __FILE__ << ":" << __LINE__ << "  CHECK(" #expr ")\n"; } } while (0)

#define CHECK_EQ(actual, expected)                                                              \
    do { const auto a_ = (actual); const auto e_ = (expected);                                          \
        if (!(a_ == e_)) { ++testing::Failures();                                               \
            std::cerr << "    FAIL " << __FILE__ << ":" << __LINE__ << "  " #actual " == " #expected \
                      << "\n          got: " << testing::Show(a_) << "\n     expected: " << testing::Show(e_) << "\n"; } } while (0)

#define CHECK_THROWS_AS(expr, ExType)                                                           \
    do { bool caught_ = false;                                                                  \
        try { (void)(expr); } catch (const ExType&) { caught_ = true; } catch (...) {}          \
        if (!caught_) { ++testing::Failures();                                                  \
            std::cerr << "    FAIL " << __FILE__ << ":" << __LINE__ << "  expected " #ExType " from " #expr "\n"; } } while (0)
