#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "uzuki/validate.hpp"
#include "nlohmann/json.hpp"

void quick_check(std::string contents, std::string msg, int nexpected = 0) {
    nlohmann::json mocked = nlohmann::json::parse(contents);
    EXPECT_ANY_THROW({
        try {
            uzuki::validate(mocked, nexpected);
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(BasicListTest, Structural) {
    quick_check("1", "arrays or object");

    // This is not correct as the top-level object should not be an
    // endpoint object, i.e., pointing to a specific value.
    quick_check("{ \"type\": \"string\", \"values\": [ \"a\", \"b\"] }", "top-level");

    quick_check("[{ \"type\": 1, \"values\": [ \"a\", \"b\"] }]", "object, array or string"); 
}

TEST(BasicListTest, ElementChecks) {
    quick_check("[{ \"type\": \"string\", \"values\": [1, 2, 3] }]", "should be a string");
    quick_check("[{ \"type\": \"date\", \"values\": [\"a\", \"b\"] }]", "YYYY-MM-DD");
    quick_check("[{ \"type\": \"number\", \"values\": [\"a\", 2, 3] }]", "should be a number");
    quick_check("[{ \"type\": \"integer\", \"values\": [1.5, 2, 3] }]", "should be an integer");
    quick_check("[{ \"type\": \"integer\", \"values\": [12345678901] }]", "32-bit integer");
    quick_check("[{ \"type\": \"boolean\", \"values\": [1, true, false] }]", "should be a boolean");

    // Factors need their own checks.
    quick_check("[{ \"type\": \"factor\", \"values\": [\"a\", \"b\"] }]", "levels");
    quick_check("[{ \"type\": \"factor\", \"values\": [\"a\", \"b\"], \"levels\": [ 1 ] }]", "levels");
    quick_check("[{ \"type\": \"factor\", \"values\": [\"a\", \"b\"], \"levels\": [ \"A\" ] }]", "levels");
}

TEST(BasicListTest, OtherChecks) {
    quick_check("[{ \"type\": \"other\" }]", "index");
    quick_check("[{ \"type\": \"other\", \"index\": \"asdasd\" }]", "should be a number");
    quick_check("[{ \"type\": \"other\", \"index\": 1.2 }]", "non-negative");
    quick_check("[{ \"type\": \"other\", \"index\": -1 }]", "non-negative");
    quick_check("[{ \"type\": \"other\", \"index\": 1 }]", "out of range", 1);
    quick_check("[{ \"type\": \"other\", \"index\": 0 }]", "fewer", 2);
    quick_check("[{ \"type\": \"other\", \"index\": 0 }, { \"type\": \"other\", \"index\": 0 } ]", "should be consecutive", 2);
}

TEST(BasicListTest, VectorNameChecks) {
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3], \"names\": 1}]", "[0].names");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3], \"names\": [\"A\", \"B\"] }]", "an array of length 3");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3], \"names\": [\"A\", \"B\", null] }]", "should be a string");
}

TEST(BasicListTest, DimensionChecks) {
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": []}]", "non-empty array");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": [true, false]}]", "non-negative integer");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": [2.3, 1.2]}]", "non-negative integer");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": [3, 1]}]", "product");
}

TEST(BasicListTest, DimnameChecks) {
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": [3, 2], \"names\": []}]", "array of length");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": [3, 2], \"names\": [[], null]}]", "array of length 3");
    quick_check("[{ \"type\": \"integer\", \"values\": [1, 2, 3, 4, 5, 6], \"dimensions\": [3, 2], \"names\": [null, [\"A\", null]]}]", "string");
}

TEST(BasicListTest, DataFrameChecks) {
    quick_check("[{ \"type\": \"data.frame\" }]", "should be an integer");
    quick_check("[{ \"type\": \"data.frame\", \"rows\": 5 }]", "should be an object");
    quick_check("[{ \"type\": \"data.frame\", \"rows\": 5, \"columns\": { \"foo\": { \"type\": 3, \"values\": [ 1, 2, 3, 4] } } }]", "should be a string");
    quick_check("[{ \"type\": \"data.frame\", \"rows\": 5, \"columns\": { \"foo\": { \"type\": \"integer\", \"values\": [ 1, 2, 3, 4] } } }]", "not consistent");
    quick_check("[{ \"type\": \"data.frame\", \"rows\": 2, \"columns\": { \"foo\": { \"type\": \"integer\", \"values\": [ 1, 2 ]} }, \"names\": []}]", "length");
}

void quick_check(std::string contents, int num) {
    nlohmann::json mocked = nlohmann::json::parse(contents);
    EXPECT_NO_THROW(uzuki::validate(mocked, num));
} 

TEST(BasicListTest, SimpleSuccess) {
    // Some simple cases.
    quick_check("[ { \"type\": \"string\", \"values\": [\"a\"] } ]", 0);
    quick_check("[ { \"type\": \"string\", \"values\": [\"a\"], \"names\":[\"x\"] } ]", 0);

    quick_check("[ { \"type\": \"integer\", \"values\": [1,2,3,4,5,6,7,8], \"dimensions\":[2, 4] } ]", 0);
    quick_check("[ { \"type\": \"integer\", \"values\": [1,2,3,4,5,6,7,8], \"dimensions\":[2, 4], \"names\":[[\"A\", \"B\"], null]} ]", 0);
    quick_check("[ { \"type\": \"integer\", \"values\": [1,2,3,4,5,6,7,8], \"dimensions\":[2, 4], \"names\":[[\"A\", \"B\"], [\"a\", \"b\", \"c\", \"d\"]]} ]", 0);

    // Trying out some other types.
    quick_check("[ { \"type\": \"date\", \"values\": [\"2020-02-21\", \"2021-03-12\"] }]", 0);
    quick_check("[ { \"type\": \"number\", \"values\": [1.5,2.1,3.2] } ]", 0);
    quick_check("[ { \"type\": \"boolean\", \"values\": [true,false] } ]", 0);
    quick_check("[ { \"type\": \"factor\", \"values\": [\"y\",\"z\"], \"levels\": [\"x\",\"y\",\"z\"] } ]", 0);
    quick_check("[ { \"type\": \"nothing\" } ]", 0);

    // And again with objects as the top-level object.
    quick_check("{ \"BLAH\":  { \"type\": \"string\", \"values\": [\"a\"] }, \"FOO\": { \"type\": \"integer\", \"values\": [1,2,3] } }", 0);
}

TEST(BasicListTest, HarderSuccess) {
    // A more complex case with deeper nesting.
    nlohmann::json thing {
        {
            "type", nlohmann::json::object({ // 'type' is structural here.
                { "type", "string" },
                { "values", nlohmann::json::array({ "asdasd", "q2ewre" }) }
            }),
        },
        {
            "values", nlohmann::json::array({
                nlohmann::json::object({
                    { "type", "integer" },
                    { "values", nlohmann::json::array({ 1, 2, 3 }) }
                }),
                nlohmann::json::object({
                    { "type", "other" },
                    { "index", 0 }
                }),
                nlohmann::json::object({ 
                    { "type", "other" },
                    { "index", 1 }
                })
            })
        }
    };
    EXPECT_NO_THROW(uzuki::validate(thing, 2));
}

TEST(BasicListTest, DataFrameSuccess) {
    nlohmann::json thing = nlohmann::json::array({
        {
            { "type", "data.frame" },
            { "rows", 2 },
            { 
                "columns", nlohmann::json::object({
                    { 
                        "foobar", nlohmann::json::object({ // 'type' is structural here.
                            { "type", "string" },
                            { "values", nlohmann::json::array({ "asdasd", "q2ewre" }) }
                        })
                    },
                    {
                        "rabbid", nlohmann::json::object({ // 'type' is structural here.
                            { "type", "integer" },
                            { "values", nlohmann::json::array({ 2, 4 }) }
                        })
                    }
                })
            },
            { "names", nlohmann::json::array({ "Alpha", "Bravo" }) }
        }
    });
    EXPECT_NO_THROW(uzuki::validate(thing));
}
