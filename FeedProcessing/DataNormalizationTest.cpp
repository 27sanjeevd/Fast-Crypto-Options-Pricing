#include "DataNormalization.h"
#include <gtest/gtest.h>

class DataNormalizationTest : public ::testing::Test {
  protected:
    DataNormalization normalizer;
};

TEST_F(DataNormalizationTest, ParseBookDeltaMessage) {
    std::string json_str =
        R"({"id":-1,"method":"subscribe","code":0,"result":{"instrument_name":"ETHUSD-PERP","subscription":"book.ETHUSD-PERP.10","channel":"book.update","depth":10,"data":[{"update":{"asks":[["4457.07","4.4444","8"]],"bids":[]},"t":1758447954211,"tt":1758447954207,"u":249977186042272,"pu":249977185983904,"cs":590576900}]}})";

    auto result = normalizer.ParseExchangeMessage(json_str);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->type, ExchangeTypes::MessageType::BOOK_DELTA_UPDATE);
    ASSERT_TRUE(result->book_delta.has_value());

    auto &delta = result->book_delta.value();
    EXPECT_EQ(delta.result.instrument_name, "ETHUSD-PERP");
    EXPECT_EQ(delta.result.depth, 10);
    EXPECT_EQ(delta.result.channel, "book.update");
}

TEST_F(DataNormalizationTest, ParseInvalidMessage) {
    std::string json_str = "invalid json";

    auto result = normalizer.ParseExchangeMessage(json_str);

    EXPECT_FALSE(result.has_value());
}
