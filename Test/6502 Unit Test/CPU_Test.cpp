#include <cstdio>
#include "gtest/gtest.h"
#include "../TEST_ENTRY.h"
#include "LoadStoreOpt/LDA_TEST.h"
#include "LoadStoreOpt/LDX_TEST.h"
#include "LoadStoreOpt/LDY_TEST.h"

GTEST_API_ int main(int argc, char** argv) {
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

