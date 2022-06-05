#include <gtest/gtest.h>
#include "source.h"

using namespace NKaleidoscope;

TEST(SourceTest, FromString) {
    auto s = TSource::FromString("def sample");
    EXPECT_TRUE(s.GetBuffer() == "def sample");
    EXPECT_TRUE(s.GetFileName() == nullptr);
}

TEST(SourceTest, SourceRange) {
    auto s = TSource::FromString("def sample");
    TSourceRange sr{.Source = &s, .Offset = 2, .Length = 4};
    EXPECT_TRUE(sr.AsStringView() == "f sa");
}
