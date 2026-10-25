#include "TestFramework.hpp"

int main() {
    int failedCases = 0;
    for (const auto& test : testing::Registry()) {
        const int before = testing::Failures();
        try {
            test.run();
        } catch (const std::exception& e) {
            ++testing::Failures();
            std::cerr << "    FAIL unexpected exception: " << e.what() << "\n";
        }
        const bool ok = testing::Failures() == before;
        std::cout << (ok ? "[ ok ] " : "[FAIL] ") << test.name << "\n";
        if (!ok) ++failedCases;
    }
    std::cout << "\n" << testing::Registry().size() << " tests, " << failedCases << " failed\n";
    return failedCases == 0 ? 0 : 1;
}
