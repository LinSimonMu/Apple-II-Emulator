#include <cstdio>
#include "gtest/gtest.h"
#include "6502 Unit Test/LDA/LDA_TEST.h"


GTEST_API_ int main(int argc, char** argv) {
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

