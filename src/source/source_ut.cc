#include <gtest/gtest.h>
#include "source.h"

using namespace NKaleidoscope;

TEST(SourceTest, FromString) {
    auto s = TSource::FromString("def sample");
    EXPECT_TRUE(s.GetBuffer() == "def sample");
    EXPECT_TRUE(s.GetFileName() == nullptr);
}
