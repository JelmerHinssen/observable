#include <gtest/gtest.h>
#include <iostream>

using namespace testing;
using namespace std;

namespace testing {
class listener : public TestEventListener {
 public:
  void OnTestProgramStart(const UnitTest& /*unit_test*/) override {}
  void OnTestIterationStart(const UnitTest& /*unit_test*/,
                            int /*iteration*/) override {}
  void OnEnvironmentsSetUpStart(const UnitTest& /*unit_test*/) override {}
  void OnEnvironmentsSetUpEnd(const UnitTest& /*unit_test*/) override {}
  void OnTestSuiteStart(const TestSuite& /*test_suite*/) override {}
//  Legacy API is deprecated but still available
#ifndef GTEST_REMOVE_LEGACY_TEST_CASEAPI_
  void OnTestCaseStart(const TestCase& /*test_case*/) override {}
#endif

  void OnTestStart(const TestInfo& /*test_info*/) override {}
  void OnTestPartResult(const TestPartResult& /*test_part_result*/) override {}
  void OnTestEnd(const TestInfo& /*test_info*/) override {}
  void OnTestSuiteEnd(const TestSuite& /*test_suite*/) override {}
#ifndef GTEST_REMOVE_LEGACY_TEST_CASEAPI_
  void OnTestCaseEnd(const TestCase& /*test_case*/) override {}
#endif

  void OnEnvironmentsTearDownStart(const UnitTest& /*unit_test*/) override {}
  void OnEnvironmentsTearDownEnd(const UnitTest& /*unit_test*/) override {}
  void OnTestIterationEnd(const UnitTest& /*unit_test*/,
                          int /*iteration*/) override {}
  void OnTestProgramEnd(const UnitTest& /*unit_test*/) override {}
};
}

/// A test result printer that's less wordy than gtest's default.
class LaconicPrinter : public testing::EmptyTestEventListener {
public:
    LaconicPrinter(testing::TestEventListener* listener) : 
        listener(listener),
        have_blank_line_(false), 
        smart_terminal_(true) {}

    virtual void OnTestStart(const testing::TestInfo& /*test_info*/) {
    }

    virtual void OnTestPartResult(
        const testing::TestPartResult& test_part_result) {
        if (!test_part_result.failed())
            return;
        listener->OnTestPartResult(test_part_result);
    }

    virtual void OnTestEnd(const testing::TestInfo& test_info) {
        if (test_info.result()->Failed()) {
            cout << "Test failed: " << test_info.name() << endl;
        }
    }

    virtual void OnTestProgramEnd(const testing::UnitTest& /*unit_test*/) {
        if (!have_blank_line_ && smart_terminal_)
            printf("\n");
    }

    void OnTestSuiteEnd(const testing::TestSuite& test_suite) override {
        listener->OnTestSuiteEnd(test_suite);
    }
    
    ~LaconicPrinter() {
        delete listener;
    }

    void OnTestProgramStart(const UnitTest& unit_test) override {
        listener->OnTestProgramStart(unit_test);
    }
    void OnTestIterationStart(const UnitTest& unit_test,
                            int iteration) override {
        listener->OnTestIterationStart(unit_test, iteration);
    }
    void OnEnvironmentsSetUpStart(const UnitTest& unit_test) override {
        listener->OnEnvironmentsSetUpStart(unit_test);
    }
    void OnEnvironmentsSetUpEnd(const UnitTest& unit_test) override {
        listener->OnEnvironmentsSetUpEnd(unit_test);
    }
    void OnTestSuiteStart(const TestSuite& /*test_suite*/) override {
    }
    //  Legacy API is deprecated but still available
#ifndef GTEST_REMOVE_LEGACY_TEST_CASEAPI_
    void OnTestCaseStart(const TestCase& /*test_case*/) override {
    }
#endif

#ifndef GTEST_REMOVE_LEGACY_TEST_CASEAPI_
    void OnTestCaseEnd(const TestCase& test_case) override {
        if (test_case.failed_test_count() != 0)
            listener->OnTestCaseEnd(test_case);
    }
#endif

    void OnEnvironmentsTearDownStart(const UnitTest& unit_test) override {
        listener->OnEnvironmentsTearDownStart(unit_test);
    }
    void OnEnvironmentsTearDownEnd(const UnitTest& unit_test) override {
        listener->OnEnvironmentsTearDownEnd(unit_test);
    }
    void OnTestIterationEnd(const UnitTest& unit_test,
                            int iteration) override {
        listener->OnTestIterationEnd(unit_test, iteration);
    }

private:
    bool have_blank_line_;
    bool smart_terminal_;
    testing::TestEventListener* listener;
};

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  testing::TestEventListeners& listeners =
      testing::UnitTest::GetInstance()->listeners();
  auto listener = listeners.Release(listeners.default_result_printer());
  listeners.Append(new LaconicPrinter(listener));
  int res = RUN_ALL_TESTS();
  return res;
}