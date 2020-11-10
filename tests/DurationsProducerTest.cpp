// NB: The Time Domain Producer takes both a duration protocol and a number
// protocol. These tests concentrate on testing the duration protocols in-depth
// and do not put much emphasis on the number protocols. This is because
// duration protocols are not put through extensive integration tests anywhere
// else, whereas number protocols are. So going in-depth with number protocols
// would just be a duplication of the work done in the NumbersIntegrationTests.

// In fact, these tests use just the Cycle number protocol as a way to get the
// items from the duration protocols in a way that is uniform across all
// duration protocols under test. It is used because it is the most
// predictable of all the number protocols.

#include "DurationsProducer.hpp"

#include "Geometric.hpp"
#include "Multiples.hpp"
#include "Prescribed.hpp"
#include "Range.hpp"
#include "UniformGenerator.hpp"

#include <catch2/catch.hpp>

SCENARIO("TimeDomain: Using Prescribed and Cycle")
{
    using namespace aleatoric;

    std::vector<int> sourceDurations {1, 2, 3, 4, 5};

    DurationsProducer instance(
        std::make_unique<Prescribed>(sourceDurations),
        NumberProtocol::create(NumberProtocol::Type::cycle));

    WHEN("A sample has been gathered that matches the size of the duration "
         "collection")
    {
        auto sample = instance.getCollection(sourceDurations.size());

        THEN("The result should match the provided source durations collection")
        {
            REQUIRE(sample == sourceDurations);
        }
    }
}

SCENARIO("TimeDomain: Using Multiples and Cycle")
{
    using namespace aleatoric;

    GIVEN("The duration protocol is configured with a range and no deviation "
          "factor")
    {
        int baseIncrement = 100;
        Range range(11, 13);

        DurationsProducer instance(
            std::make_unique<Multiples>(baseIncrement, range),
            NumberProtocol::create(NumberProtocol::Type::cycle));

        WHEN("A sample has been gathered that matches the size of the range")
        {
            auto sample = instance.getCollection(range.size);

            THEN("The sample should include each increment in the range "
                 "multiplied by the base increment")
            {
                std::vector<int> expectedResult {1100, 1200, 1300};
                REQUIRE(sample == expectedResult);
            }
        }
    }

    GIVEN("The duration protocol is configured with a range a deviation factor")
    {
        int baseIncrement = 100;
        double deviationFactor = 0.1;
        Range range(10, 20);

        // TODO: TIME DOMAIN: this is a lot for a caller to have to set up. Is
        // there a need for a factory here? Don't think a caller should have to
        // worry about sending in a Uniform Generator in order to create an
        // instance of a duration protocol.
        DurationsProducer instance(
            std::make_unique<Multiples>(baseIncrement,
                                        range,
                                        deviationFactor,
                                        std::make_unique<UniformGenerator>()),
            NumberProtocol::create(NumberProtocol::Type::cycle));

        WHEN("A sample has been gathered that matches the size of the range")
        {
            auto sample = instance.getCollection(range.size);

            THEN("The sample should include a number ± the deviation factor of "
                 "each increment in the range, first having been multiplied by "
                 "the base increment")
            {
                std::vector<int> expectedResultWithoutDeviation {1000,
                                                                 1100,
                                                                 1200,
                                                                 1300,
                                                                 1400,
                                                                 1500,
                                                                 1600,
                                                                 1700,
                                                                 1800,
                                                                 1900,
                                                                 2000};

                for(int i = 0; i < sample.size(); i++) {
                    auto expectedItem = expectedResultWithoutDeviation[i];

                    auto deviation = expectedItem * deviationFactor;

                    auto minDeviation = expectedItem - deviation;

                    auto maxDeviation = expectedItem + deviation;

                    REQUIRE(sample[i] >= minDeviation);
                    REQUIRE(sample[i] <= maxDeviation);
                }
            }
        }
    }

    GIVEN("The duration protocol is configured with a multipliers collection "
          "and no deviation factor")
    {
        int baseIncrement = 100;
        std::vector<int> multipliers {10, 11, 12};

        DurationsProducer instance(
            std::make_unique<Multiples>(baseIncrement, multipliers),
            NumberProtocol::create(NumberProtocol::Type::cycle));

        WHEN("A sample has been gathered that matches the size of the "
             "multiples collection")
        {
            auto sample = instance.getCollection(multipliers.size());

            THEN("The sample should include each item in the multiples "
                 "collection multiplied by the base increment")
            {
                std::vector<int> expectedResult {1000, 1100, 1200};

                REQUIRE(sample == expectedResult);
            }
        }
    }

    GIVEN("The duration protocol is configured with a multipliers collection "
          "a deviation factor")
    {
        int baseIncrement = 100;
        std::vector<int>
            multipliers {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        double deviationFactor = 0.1;

        // TODO: TIME DOMAIN: same issue as above
        DurationsProducer instance(
            std::make_unique<Multiples>(baseIncrement,
                                        multipliers,
                                        deviationFactor,
                                        std::make_unique<UniformGenerator>()),
            NumberProtocol::create(NumberProtocol::Type::cycle));

        WHEN("A sample has been gathered that matches the size of the "
             "multiples collection")
        {
            auto sample = instance.getCollection(multipliers.size());

            THEN("The sample should include a number ± the deviation factor of "
                 "each increment in the multipliers collection, first having "
                 "been multiplied by the base increment")
            {
                std::vector<int> expectedResultWithoutDeviation {1000,
                                                                 1100,
                                                                 1200,
                                                                 1300,
                                                                 1400,
                                                                 1500,
                                                                 1600,
                                                                 1700,
                                                                 1800,
                                                                 1900,
                                                                 2000};

                for(int i = 0; i < sample.size(); i++) {
                    auto expectedItem = expectedResultWithoutDeviation[i];

                    auto deviation = expectedItem * deviationFactor;

                    auto minDeviation = expectedItem - deviation;

                    auto maxDeviation = expectedItem + deviation;

                    REQUIRE(sample[i] >= minDeviation);
                    REQUIRE(sample[i] <= maxDeviation);
                }
            }
        }
    }
}

SCENARIO("TimeDomain: Using Geometric and Cycle")
{
    using namespace aleatoric;

    GIVEN("The duration protocol is configured with a range and a collection "
          "size")
    {
        Range range(256, 4096);
        int collectionSize = 5;

        DurationsProducer instance(
            std::make_unique<Geometric>(range, collectionSize),
            NumberProtocol::create(NumberProtocol::Type::cycle));

        WHEN("A sample is gathered that is the size of the collection size "
             "argument")
        {
            auto sample = instance.getCollection(collectionSize);

            THEN("The sample should should match the expected geometric "
                 "sequence")
            {
                // NB: The following is lifted directly from the unit tests for
                // Geometric!

                // When the supplied parameters are:
                // range: 256 - 4096
                // collectionSize: 5
                // Then this will internally have a
                // common ratio of 2 (see class implementation for details).
                // Therefore the geometric sequence is calculated as: rangeStart
                // * cr^n-term

                // And the sequence will be as follows:
                /*
                0: 256 * 2^0 = 256 (2^0 = 1)
                1: 256 * 2^1 = 512 (2^1 = 2)
                2: 256 * 2^2 = 1024 (2^2 = 4)
                3: 256 * 2^3 = 2048 (2^3 = 8)
                4: 256 * 2^4 = 4096 (2^4 = 16)
                */

                std::vector<int> expectedSequence {256, 512, 1024, 2048, 4096};

                REQUIRE(sample == expectedSequence);
            }
        }
    }
}

SCENARIO("TimeDomain: Get and set params (using Prescribed and Cycle for test)")
{
    using namespace aleatoric;

    std::vector<int> sourceDurations {1, 2, 3, 4, 5};

    DurationsProducer instance(
        std::make_unique<Prescribed>(sourceDurations),
        NumberProtocol::create(NumberProtocol::Type::cycle));

    GIVEN("Params have not been updated")
    {
        WHEN("Get initial params")
        {
            THEN("Params should reflect correct state")
            {
                auto params = instance.getParams();
                auto cycleParams = params.getCycle();
                REQUIRE(
                    params.getActiveProtocol() ==
                    NumberProtocolParameters::Protocols::ActiveProtocol::cycle);
                REQUIRE_FALSE(cycleParams.getReverseDirection());
                REQUIRE_FALSE(cycleParams.getBidirectional());
            }
        }

        WHEN("Request a pair of cycles through durations")
        {
            std::vector<int> expectedResult {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
            auto sample = instance.getCollection(expectedResult.size());

            THEN("Result should be as expected")
            {
                REQUIRE(sample == expectedResult);
            }
        }
    }

    GIVEN("Params have been updated: change to reverse unidirectional")
    {
        NumberProtocolParameters::Protocols newParams(
            NumberProtocolParameters::Cycle(false, true));

        instance.setParams(newParams);

        THEN("Params reflect update")
        {
            auto params = instance.getParams();
            auto cycleParams = params.getCycle();
            REQUIRE(cycleParams.getReverseDirection());
            REQUIRE_FALSE(cycleParams.getBidirectional());
        }

        WHEN("A pair of cycles is requested")
        {
            std::vector<int> expectedResult {5, 4, 3, 2, 1, 5, 4, 3, 2, 1};
            auto sample = instance.getCollection(expectedResult.size());

            THEN("Result is the reverse of the original sample")
            {
                REQUIRE(sample == expectedResult);
            }
        }
    }

    WHEN("User provides the wrong protocol params")
    {
        THEN("Exception is thrown")
        {
            // Provides Basic protocol params, not cycle
            REQUIRE_THROWS_AS(
                instance.setParams(NumberProtocolParameters::Protocols(
                    NumberProtocolParameters::Basic())),
                std::invalid_argument);
        }
    }
}

SCENARIO("Timedomain: Change number protocol")
{
    using namespace aleatoric;

    std::vector<int> sourceDurations {1, 2, 3};

    DurationsProducer instance(
        std::make_unique<Prescribed>(sourceDurations),
        NumberProtocol::create(NumberProtocol::Type::cycle));

    WHEN("Before number protocol change")
    {
        THEN("Active protocol is as expected")
        {
            auto activeProtocol = instance.getParams().getActiveProtocol();
            REQUIRE(activeProtocol ==
                    NumberProtocolParameters::Protocols::ActiveProtocol::cycle);
        }

        THEN("Set of durations is as expected")
        {
            instance.setParams(NumberProtocolParameters::Protocols(
                NumberProtocolParameters::Cycle(false, false)));

            std::vector<int> expectedResult {1, 2, 3, 1, 2, 3};
            auto set = instance.getCollection(expectedResult.size());

            REQUIRE(set == expectedResult);
        }
    }

    WHEN("After number protocol change")
    {
        instance.setNumberProtocol(
            NumberProtocol::create(NumberProtocol::Type::periodic));

        THEN("Active protocol is as expected")
        {
            auto activeProtocol = instance.getParams().getActiveProtocol();
            REQUIRE(
                activeProtocol ==
                NumberProtocolParameters::Protocols::ActiveProtocol::periodic);
        }

        THEN("Set of durations is as expected")
        {
            instance.setParams(NumberProtocolParameters::Protocols(
                NumberProtocolParameters::Periodic(1.0)));

            // use as reference duration for set gathered next
            auto firstDuration = instance.getDuration();

            for(int i = 0; i < 1000; i++) {
                REQUIRE(instance.getDuration() == firstDuration);
            }
        }
    }
}

SCENARIO("Timedomain: Change duration protocol")
{
    using namespace aleatoric;

    std::vector<int> sourceDurations {1, 2, 3};

    DurationsProducer instance(
        std::make_unique<Prescribed>(sourceDurations),
        NumberProtocol::create(NumberProtocol::Type::cycle));

    // NB: set to reverse bidirectional
    instance.setParams(NumberProtocolParameters::Protocols(
        NumberProtocolParameters::Cycle(true, true)));

    WHEN("Before duration protocol change")
    {
        THEN("Output as expected")
        {
            std::vector<int> expectedResult {3, 2, 1, 2, 3};
            auto set = instance.getCollection(expectedResult.size());
            REQUIRE(set == expectedResult);
        }
    }

    WHEN("After duration protocol change: no collection size change")
    {
        instance.setDurationProtocol(
            std::make_unique<Multiples>(10, Range(1, 3)));

        THEN("Output is changed")
        {
            std::vector<int> expectedResult {30, 20, 10, 20, 30};
            auto set = instance.getCollection(expectedResult.size());
            REQUIRE(set == expectedResult);
        }

        THEN("Number protocol params are unchanged")
        {
            auto cycleParams = instance.getParams().getCycle();
            REQUIRE(cycleParams.getBidirectional() == true);
            REQUIRE(cycleParams.getReverseDirection() == true);
        }
    }

    WHEN("After duration protocol change: collection size (and thus range) "
         "change")
    {
        instance.setDurationProtocol(
            std::make_unique<Multiples>(10, Range(1, 5)));

        THEN("Output is changed: number protocol is set to default params (due "
             "to range change)")
        {
            // cycle default is: unidirectional, forwards
            std::vector<int>
                expectedResult {10, 20, 30, 40, 50, 10, 20, 30, 40, 50};
            auto set = instance.getCollection(expectedResult.size());
            REQUIRE(set == expectedResult);
        }

        THEN("Number protocol params are changed to defaults")
        {
            // cycle default is: unidirectional, forwards
            auto cycleParams = instance.getParams().getCycle();
            REQUIRE_FALSE(cycleParams.getBidirectional());
            REQUIRE_FALSE(cycleParams.getReverseDirection());
        }
    }
}