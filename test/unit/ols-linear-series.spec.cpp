#include "catch_amalgamated.hpp"

#include "../../src/utils/ols-linear-series.h"
#include "regression.test-cases.spec.h"

TEST_CASE("Ordinary Least Square Linear Regression", "[regression]")
{
    OLSLinearSeries olsReg(testMaxSize);

    for (auto const &testCase : testCases)
    {
        olsReg.push(testCase[0], testCase[1]);
    }

    SECTION("should correctly calculate the slope")
    {
        auto const slopeExpected = -0.0257004818;
        CHECK_THAT(olsReg.slope(), Catch::Matchers::WithinRel(slopeExpected, 0.00001));
    }

    SECTION("should correctly calculate the goodness of fit")
    {
        auto const goodnessOfFitExpected = 0.9961418613;
        CHECK_THAT(olsReg.goodnessOfFit(), Catch::Matchers::WithinRel(goodnessOfFitExpected, 0.00001));
    }
}
