#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "uzuki/parse.hpp"
#include "nlohmann/json.hpp"

#include "test_subclass.h"

auto load(std::string contents, int nexpected) {
    nlohmann::json stuff = nlohmann::json::parse(contents);
    DefaultExternals ext(nexpected);
    return uzuki::parse<DefaultProvisioner>(stuff, ext);
}

TEST(LoadTest, StringVectorCheck) {
    auto out = load("[{ \"type\": \"string\", \"values\": [\"A\", \"BC\", \"DEF\"] }, { \"type\": \"string\", \"values\": [ null ] } ]", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 2);
    EXPECT_FALSE(lptr->has_names);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::STRING);
    EXPECT_EQ(lptr->values[1]->type(), uzuki::STRING);

    auto ptr = static_cast<const DefaultStringVector*>(lptr->values[0].get());
    EXPECT_EQ(ptr->size(), 3);
    EXPECT_EQ(ptr->base.values[0], "A");
    EXPECT_EQ(ptr->base.values[1], "BC");
    EXPECT_EQ(ptr->base.values[2], "DEF");

    auto ptr2 = static_cast<const DefaultStringVector*>(lptr->values[1].get());
    EXPECT_EQ(ptr2->size(), 1);
    EXPECT_EQ(ptr2->base.values[0], "ich bin missing");
}

TEST(LoadTest, NumberVectorCheck) {
    auto out = load("{ \"double\": { \"type\": \"number\", \"values\": [ null, -1.2, 4.9  ] }, \"integer\": {\"type\": \"integer\", \"values\": [ 0, 1, 2, null ] } }", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 2);
    EXPECT_TRUE(lptr->has_names);

    EXPECT_EQ(lptr->names[0], "double");
    EXPECT_EQ(lptr->values[0]->type(), uzuki::NUMBER);

    {
        auto ptr = static_cast<const DefaultNumberVector*>(lptr->values[0].get());
        EXPECT_EQ(ptr->size(), 3);
        EXPECT_TRUE(std::isnan(ptr->base.values[0]));
        EXPECT_EQ(ptr->base.values[1], -1.2);
        EXPECT_EQ(ptr->base.values[2], 4.9);
    }

    EXPECT_EQ(lptr->names[1], "integer");
    EXPECT_EQ(lptr->values[1]->type(), uzuki::INTEGER);

    {
        auto ptr = static_cast<const DefaultIntegerVector*>(lptr->values[1].get());
        EXPECT_EQ(ptr->size(), 4);
        EXPECT_EQ(ptr->base.values[0], 0);
        EXPECT_EQ(ptr->base.values[1], 1);
        EXPECT_EQ(ptr->base.values[2], 2);
        EXPECT_EQ(ptr->base.values[3], std::numeric_limits<int32_t>::min());
    }
}

TEST(LoadTest, BooleanCheck) {
    auto out = load("[ { \"type\": \"boolean\", \"values\": [ true, false, null ], \"names\": [ \"x\", \"yz\", \"abc\" ] } ]", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_FALSE(lptr->has_names);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::BOOLEAN);

    auto ptr = static_cast<const DefaultBooleanVector*>(lptr->values[0].get());
    EXPECT_EQ(ptr->size(), 3);
    EXPECT_TRUE(ptr->base.values[0]);
    EXPECT_FALSE(ptr->base.values[1]);
    EXPECT_EQ(ptr->base.values[2], 255);

    EXPECT_EQ(ptr->base.names[0], "x");
    EXPECT_EQ(ptr->base.names[1], "yz");
    EXPECT_EQ(ptr->base.names[2], "abc");
}

TEST(LoadTest, DateVectorCheck) {
    auto out = load("[ { \"type\": \"date\", \"values\": [ \"2022-05-21\", \"2017-06-22\" ] } ]", 0); 
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_FALSE(lptr->has_names);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::DATE);

    auto ptr = static_cast<const DefaultDateVector*>(lptr->values[0].get());
    EXPECT_EQ(ptr->size(), 2);
    EXPECT_EQ(ptr->base.values[0], "2022-05-21");
    EXPECT_EQ(ptr->base.values[1], "2017-06-22");
}

TEST(LoadTest, FactorCheck) {
    auto out = load("{ \"factor\": { \"type\": \"factor\", \"values\": [ \"aaron\", \"natalie portman\", \"aaron\" ], \"levels\": [ \"aaron\", \"natalie portman\" ] }, \
                      \"ordered\": { \"type\": \"ordered\", \"values\": [ \"x\", \"y\", \"z\" ], \"levels\": [ \"z\", \"y\", \"x\" ], \"names\": [ \"X\", \"Y\", \"Z\" ] } }", 0); 
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 2);
    EXPECT_TRUE(lptr->has_names);
    EXPECT_EQ(lptr->names[0], "factor");
    EXPECT_EQ(lptr->values[0]->type(), uzuki::FACTOR);
    EXPECT_EQ(lptr->names[1], "ordered");
    EXPECT_EQ(lptr->values[1]->type(), uzuki::FACTOR);

    auto ptr = static_cast<const DefaultFactor*>(lptr->values[0].get());
    EXPECT_EQ(ptr->size(), 3);
    EXPECT_FALSE(ptr->fbase.ordered);
    EXPECT_EQ(ptr->vbase.values[0], 0);
    EXPECT_EQ(ptr->vbase.values[1], 1);
    EXPECT_EQ(ptr->vbase.values[2], 0);
    EXPECT_EQ(ptr->fbase.levels[0], "aaron");
    EXPECT_EQ(ptr->fbase.levels[1], "natalie portman");

    auto ptr2 = static_cast<const DefaultFactor*>(lptr->values[1].get());
    EXPECT_EQ(ptr2->size(), 3);
    EXPECT_TRUE(ptr2->fbase.ordered);
    EXPECT_EQ(ptr2->vbase.values[0], 2);
    EXPECT_EQ(ptr2->vbase.values[1], 1);
    EXPECT_EQ(ptr2->vbase.values[2], 0);
    EXPECT_EQ(ptr2->fbase.levels[0], "z");
    EXPECT_EQ(ptr2->fbase.levels[1], "y");
    EXPECT_EQ(ptr2->fbase.levels[2], "x");
    EXPECT_EQ(ptr2->vbase.names[0], "X");
    EXPECT_EQ(ptr2->vbase.names[1], "Y");
    EXPECT_EQ(ptr2->vbase.names[2], "Z");
}

TEST(LoadTest, NumberArrayCheck) {
    auto out = load("[ { \"type\": \"number\", \"values\": [ 5.2, null, -1.2, 4.9, 2, -5 ], \"dimensions\": [ 3, 2 ] } ]", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::NUMBER_ARRAY);

    auto ptr = static_cast<const DefaultNumberArray*>(lptr->values[0].get());
    EXPECT_EQ(ptr->base.dimensions[0], 3);
    EXPECT_EQ(ptr->base.dimensions[1], 2);
    EXPECT_EQ(ptr->base.values[0], 5.2);
    EXPECT_EQ(ptr->base.values[5], -5);
}

TEST(LoadTest, NamedArrayCheck) {
    auto out = load("[ { \"type\": \"integer\", \"values\": [ 1,2,3,6,5,4 ], \"dimensions\": [ 3, 2 ], \"names\": [ null, [\"A\", \"BC\"] ] } ]", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_FALSE(lptr->has_names);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::INTEGER_ARRAY);

    auto ptr = static_cast<const DefaultIntegerArray*>(lptr->values[0].get());
    EXPECT_EQ(ptr->base.dimensions[0], 3);
    EXPECT_EQ(ptr->base.dimensions[1], 2);
    EXPECT_EQ(ptr->base.values[0], 1);
    EXPECT_EQ(ptr->base.values[5], 4);

    EXPECT_FALSE(ptr->base.has_names[0]);
    EXPECT_TRUE(ptr->base.has_names[1]);
    EXPECT_EQ(ptr->base.names[1][0], "A");
    EXPECT_EQ(ptr->base.names[1][1], "BC");
}

TEST(LoadTest, FactorArrayCheck) {
    auto out = load("[ { \"type\": \"ordered\", \"values\": [ \"jessica biel\", \"natalie portman\", \"jennifer lawrence\" ], \"levels\": [ \"jessica biel\", \"jennifer lawrence\", \"natalie portman\" ], \"dimensions\": [1, 3] } ]", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_FALSE(lptr->has_names);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::FACTOR_ARRAY);

    auto ptr = static_cast<const DefaultFactorArray*>(lptr->values[0].get());
    EXPECT_EQ(ptr->abase.dimensions[0], 1);
    EXPECT_EQ(ptr->abase.dimensions[1], 3);
    EXPECT_EQ(ptr->abase.values[0], 0);
    EXPECT_EQ(ptr->abase.values[1], 2);
    EXPECT_EQ(ptr->abase.values[2], 1);
    EXPECT_EQ(ptr->fbase.levels[0], "jessica biel");
    EXPECT_EQ(ptr->fbase.levels[1], "jennifer lawrence");
    EXPECT_EQ(ptr->fbase.levels[2], "natalie portman");
}

TEST(LoadTest, NothingCheck) {
    auto out = load("[ { \"type\": \"nothing\" }, { \"type\": \"nothing\" } ]", 0);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 2);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::NOTHING);
    EXPECT_EQ(lptr->values[1]->type(), uzuki::NOTHING);
}

TEST(LoadTest, DataFrameCheck) {
    auto out = load("[ {\
        \"type\": \"data.frame\", \
        \"rows\": 3, \
        \"columns\": { \
            \"actress\": {\"type\": \"string\", \"values\": [ \"jessica biel\", \"natalie portman\", \"jennifer lawrence\" ], \"dimensions\": [3, 1] }, \
            \"score\": {\"type\": \"integer\", \"values\": [ 8, 10, 9 ] } \
        } \
    } ]", 0);

    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::DATA_FRAME);

    auto ptr = static_cast<const DefaultDataFrame*>(lptr->values[0].get());
    EXPECT_EQ(ptr->nrows, 3);
    EXPECT_EQ(ptr->colnames[0], "actress");
    EXPECT_EQ(ptr->colnames[1], "score");
    EXPECT_FALSE(ptr->has_names);
    EXPECT_EQ(ptr->columns[0]->type(), uzuki::STRING_ARRAY);
    EXPECT_EQ(ptr->columns[1]->type(), uzuki::INTEGER);
}

TEST(LoadTest, EmptyDataFrameCheck) {
    auto out = load("[ {\
        \"type\": \"data.frame\", \
        \"rows\": 5, \
        \"columns\": {}, \
        \"names\": [ \"Natalie Portman\", \"Jennifer Lawrence\", \"Jessica Biel\", \"Scarlett Johansson\", \"Rachel Weisz\" ]  \
    } ]", 0);

    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 1);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::DATA_FRAME);

    auto ptr = static_cast<const DefaultDataFrame*>(lptr->values[0].get());
    EXPECT_EQ(ptr->colnames.size(), 0);
    EXPECT_TRUE(ptr->has_names);
    EXPECT_EQ(ptr->rownames[0], "Natalie Portman");
    EXPECT_EQ(ptr->rownames[4], "Rachel Weisz");
}

TEST(LoadTest, ExternalCheck) {
    auto out = load("[ { \"type\": \"other\", \"index\": 1 }, { \"type\": \"other\", \"index\": 0 } ]", 2);
    EXPECT_EQ(out->type(), uzuki::LIST);

    auto lptr = static_cast<const DefaultList*>(out.get());
    EXPECT_EQ(lptr->size(), 2);
    EXPECT_EQ(lptr->values[0]->type(), uzuki::OTHER);
    EXPECT_EQ(lptr->values[1]->type(), uzuki::OTHER);

    auto ptr = static_cast<const DefaultOther*>(lptr->values[0].get());
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr->ptr), 2);

    auto ptr2 = static_cast<const DefaultOther*>(lptr->values[1].get());
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2->ptr), 1);
}
