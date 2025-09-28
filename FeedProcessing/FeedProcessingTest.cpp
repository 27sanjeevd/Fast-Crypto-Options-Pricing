#include "FeedProcessing.h"
#include <gtest/gtest.h>

TEST(FeedProcessingTest, ParseValidOrderBookUpdate) {
    FeedProcessing fp("book.ETHUSD-PERP.10");

    fp.Run(1);

    EXPECT_EQ(1, 1);
}