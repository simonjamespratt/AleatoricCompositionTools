// NB: These tests are stripped-down tests in comparison to the Numbers
// Integration Tests. They will not interrogate the Protocols in the way that
// the Numbers Integretion Tests do, as that would lead to test duplication. We
// will simply check that the Collections Producers can work with the Protocols
// correctly.

#include "CollectionsProducer.hpp"

#include <catch2/catch.hpp>

SCENARIO("CollectionsProducer: Constructor")
{
    using namespace aleatoric;

    WHEN("The source size provided is too small")
    {
        THEN("Throws exception")
        {
            REQUIRE_THROWS_AS(
                CollectionsProducer<char>(
                    {},
                    NumberProtocol::create(NumberProtocol::Type::basic)),
                std::invalid_argument);

            REQUIRE_THROWS_WITH(
                CollectionsProducer<char>(
                    {},
                    NumberProtocol::create(NumberProtocol::Type::basic)),
                "The size of the source collection provided is too small. It "
                "must be two or greater");

            REQUIRE_THROWS_AS(
                CollectionsProducer<char>(
                    {'a'},
                    NumberProtocol::create(NumberProtocol::Type::basic)),
                std::invalid_argument);

            REQUIRE_THROWS_WITH(
                CollectionsProducer<char>(
                    {'a'},
                    NumberProtocol::create(NumberProtocol::Type::basic)),
                "The size of the source collection provided is too small. It "
                "must be two or greater");

            REQUIRE_NOTHROW(CollectionsProducer<char>(
                {'a', 'b'},
                NumberProtocol::create(NumberProtocol::Type::basic)));
        }
    }
}

SCENARIO("CollectionsProducer: using Basic")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("Producer initial state")
    {
        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::basic));

        WHEN("A sample has been gathered")
        {
            auto sample = instance.getCollection(1000);

            THEN("All the items in the collection should have been chosen at "
                 "least once")
            {
                for(auto &&i : source) {
                    auto findResult =
                        std::find(sample.begin(), sample.end(), i);
                    REQUIRE(findResult != sample.end());
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Cycle")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    CollectionsProducer<char> instance(
        source,
        NumberProtocol::create(NumberProtocol::Type::cycle));

    GIVEN("Producer initial state: Cycle initially in default state")
    {
        WHEN("A pair of cycles is requested")
        {
            std::vector<char> expectedResult {'a', 'b', 'c', 'a', 'b', 'c'};
            auto sample = instance.getCollection(expectedResult.size());

            THEN("The cycles should be as expected")
            {
                REQUIRE(sample == expectedResult);
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Serial")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("The Producer has been instantiated")
    {
        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::serial));

        WHEN("A full series sample set has been gathered")
        {
            auto sample = instance.getCollection(source.size());

            THEN("The sample should include every item from the source "
                 "collection and only once")
            {
                for(auto &&i : source) {
                    int count = std::count(sample.begin(), sample.end(), i);
                    REQUIRE(count == 1);
                }
            }

            AND_WHEN("The next number is requested")
            {
                auto firstItemOfNextSet = instance.getItem();

                THEN("That number should appear in the previous set")
                {
                    auto numberAppears =
                        std::any_of(sample.begin(),
                                    sample.end(),
                                    [firstItemOfNextSet](int i) {
                                        return i == firstItemOfNextSet;
                                    });

                    REQUIRE(numberAppears);
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Subset")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c', 'd', 'e'};
    int subsetMin = 2;
    int subsetMax = 4;

    GIVEN("The Producer is instantiated")
    {
        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::subset));

        instance.setParams(
            NumberProtocolParams(SubsetParams(subsetMin, subsetMax)));

        WHEN("A sample has been collected")
        {
            auto sample = instance.getCollection(1000);

            std::vector<int> countResults;

            for(auto &&i : source) {
                auto count = std::count(sample.begin(), sample.end(), i);

                countResults.push_back(count);
            }

            THEN("The count for each item from the source existing within the "
                 "sample should be between the subset min and max inclusive")
            {
                int countOfExistingNumbers = 0;

                for(auto &&i : countResults) {
                    if(i > 0) {
                        countOfExistingNumbers++;
                    }
                }

                REQUIRE(countOfExistingNumbers >= subsetMin);
                REQUIRE(countOfExistingNumbers <= subsetMax);
            }

            THEN("Sum of all found count results should be the same as the "
                 "sample range, hence all items in the sample are from the "
                 "source")
            {
                int tally = std::accumulate(countResults.begin(),
                                            countResults.end(),
                                            0);
                REQUIRE(tally == sample.size());
            }
        }
    }
}

SCENARIO("CollectionsProducer: using GroupedRepetition")
{
    using namespace aleatoric;

    // NB: See the scenario in Numbers Integration Tests for GroupedRepetition
    // re. limitations of these tests.

    std::vector<char> source {'a', 'b'};
    std::vector<int> groupings {1, 2};
    int groupingsSum = 3; // sum of the above values

    GIVEN("The Producer has been instantiated")
    {
        // NB: because the sizes of the source and the groupings are the
        // same, the serial sets in GroupedRepetition will match

        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::groupedRepetition));

        instance.setParams(
            NumberProtocolParams(GroupedRepetitionParams(groupings)));

        WHEN("Two samples each consisting of a full series set has been "
             "gathered")
        {
            auto sampleOne = instance.getCollection(groupingsSum);
            auto sampleTwo = instance.getCollection(groupingsSum);

            std::vector<std::vector<char>> possibleResults {{'a', 'b', 'b'},
                                                            {'a', 'a', 'b'},
                                                            {'b', 'a', 'a'},
                                                            {'b', 'b', 'a'}};

            THEN("Each source element should be present in the form of on of "
                 "the possible groupings")
            {
                REQUIRE_THAT(sampleOne,
                             Catch::Equals(possibleResults[0]) ||
                                 Catch::Equals(possibleResults[1]) ||
                                 Catch::Equals(possibleResults[2]) ||
                                 Catch::Equals(possibleResults[3]));

                REQUIRE_THAT(sampleTwo,
                             Catch::Equals(possibleResults[0]) ||
                                 Catch::Equals(possibleResults[1]) ||
                                 Catch::Equals(possibleResults[2]) ||
                                 Catch::Equals(possibleResults[3]));
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Ratio")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("The Producer has been set with a Ratio Protocol with mixed "
          "ratio values")
    {
        std::vector<int> ratios {3, 1, 2};
        int ratioSum = 6;

        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::ratio));

        instance.setParams(NumberProtocolParams(RatioParams(ratios)));

        WHEN("A full series sample has been gathered")
        {
            auto sample = instance.getCollection(ratioSum);

            THEN("The sample should contain the right amount of each source "
                 "item")
            {
                for(int i = 0; i < source.size(); i++) {
                    auto count =
                        std::count(sample.begin(), sample.end(), source[i]);
                    REQUIRE(count == ratios[i]);
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Precision")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("The Producer has been instantiated with no initial selection")
    {
        WHEN("The distribution is uniform")
        {
            std::vector<double> distribution(source.size());

            // Make a uniform distribution
            for(auto &&i : distribution) {
                i = 1.0 / source.size();
            }

            CollectionsProducer<char> instance(
                source,
                NumberProtocol::create(NumberProtocol::Type::precision));

            instance.setParams(
                NumberProtocolParams(PrecisionParams(distribution)));

            AND_WHEN("A sample is requested")
            {
                auto sample = instance.getCollection(1000);

                THEN("All items in the sample should be from the source "
                     "collection")
                {
                    for(auto &&i : sample) {
                        REQUIRE_THAT(source, Catch::VectorContains(i));
                    }
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using NoRepetition")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("The producer has been instantiated")
    {
        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::noRepetition));

        WHEN("A sample has been gathered")
        {
            auto sample = instance.getCollection(1000);

            THEN("All items in the sample should be from the source collection")
            {
                for(auto &&i : sample) {
                    REQUIRE_THAT(source, Catch::VectorContains(i));
                }
            }

            THEN("There should be no direct or immediate repetition of numbers")
            {
                for(auto i = sample.begin(); i != sample.end(); ++i) {
                    // Don't make assertion for first number in collection as
                    // there is no previous number to compare with
                    if(i != sample.begin()) {
                        REQUIRE(*i != *std::prev(i));
                    }
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Periodic")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("The Producer has been instantiated with no initial selection")
    {
        AND_GIVEN("The chance of repetition is mid range")
        {
            double chanceOfRepetition = 0.5;

            CollectionsProducer<char> instance(
                source,
                NumberProtocol::create(NumberProtocol::Type::periodic));

            instance.setParams(
                NumberProtocolParams(PeriodicParams(chanceOfRepetition)));

            WHEN("A sample has been gathered")
            {
                auto sample = instance.getCollection(1000);

                THEN("All items in the sample should be from the source "
                     "collection")
                {
                    for(auto &&i : sample) {
                        REQUIRE_THAT(source, Catch::VectorContains(i));
                    }
                }

                THEN("At least one number should be directly repeated")
                {
                    auto firstIndexOfFoundMatchingAdjacentValues =
                        std::adjacent_find(sample.begin(), sample.end());
                    REQUIRE(firstIndexOfFoundMatchingAdjacentValues !=
                            sample.end());
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using AdjacentSteps")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    GIVEN("The Producer has been instantiated with no initial selection")
    {
        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::adjacentSteps));

        WHEN("A sample has been gathered")
        {
            auto sample = instance.getCollection(1000);

            THEN("All items in the sample should be from the source collection")
            {
                for(auto &&i : sample) {
                    REQUIRE_THAT(source, Catch::VectorContains(i));
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: using Walk")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c', 'd', 'e'};

    GIVEN("The Producer has been instantiated with no initial selection")
    {
        int maxStep = 2;

        CollectionsProducer<char> instance(
            source,
            NumberProtocol::create(NumberProtocol::Type::walk));

        instance.setParams(NumberProtocolParams(WalkParams(maxStep)));

        WHEN("A sample has been gathered")
        {
            auto sample = instance.getCollection(1000);

            THEN("All items in the sample should be from the source collection")
            {
                for(auto &&i : sample) {
                    REQUIRE_THAT(source, Catch::VectorContains(i));
                }
            }
        }
    }
}

SCENARIO("CollectionsProducer: Get and set params (using cycle for test)")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    CollectionsProducer<char> instance(
        source,
        NumberProtocol::create(NumberProtocol::Type::cycle));

    GIVEN("Params have not been updated")
    {
        WHEN("Get initial params")
        {
            THEN("Params should reflect correct state")
            {
                auto params = instance.getParams();
                auto cycleParams = params.getCycle();
                REQUIRE(params.getActiveProtocol() ==
                        NumberProtocol::Type::cycle);
                REQUIRE_FALSE(cycleParams.getReverseDirection());
                REQUIRE_FALSE(cycleParams.getBidirectional());
            }
        }

        WHEN("Request a pair of cycles")
        {
            std::vector<char> expectedResult {'a', 'b', 'c', 'a', 'b', 'c'};
            auto sample = instance.getCollection(expectedResult.size());

            THEN("The cycles should be as expected")
            {
                REQUIRE(sample == expectedResult);
            }
        }
    }

    GIVEN("Params have been updated: change to reverse unidirectional")
    {
        NumberProtocolParams newParams(CycleParams(false, true));

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
            std::vector<char> expectedResult {'c', 'b', 'a', 'c', 'b', 'a'};
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
                instance.setParams(NumberProtocolParams(BasicParams())),
                std::invalid_argument);
        }
    }
}

SCENARIO("CollectionsProducer: Change protocol")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    CollectionsProducer<char> instance(
        source,
        NumberProtocol::create(NumberProtocol::Type::cycle));

    WHEN("Before protocol change")
    {
        THEN("Active protocol is as expected")
        {
            auto activeProtocol = instance.getParams().getActiveProtocol();
            REQUIRE(activeProtocol ==
                    NumberProtocol::Type::cycle);
        }

        THEN("A collection of items is as expected")
        {
            instance.setParams(NumberProtocolParams(CycleParams(false, false)));

            std::vector<char> expectedResult {'a', 'b', 'c', 'a', 'b', 'c'};
            auto set = instance.getCollection(expectedResult.size());

            REQUIRE(set == expectedResult);
        }
    }

    WHEN("After protocol change")
    {
        instance.setProtocol(
            NumberProtocol::create(NumberProtocol::Type::serial));

        THEN("Active protocol is as expected")
        {
            auto activeProtocol = instance.getParams().getActiveProtocol();
            REQUIRE(activeProtocol ==
                    NumberProtocol::Type::serial);
        }

        THEN("Set of items is as expected with the protocol range configured "
             "to the source size")
        {
            // Get several sets matching source size - serial will return a
            // permutated version of the source for each
            std::vector<std::vector<char>> sets(10);
            for(auto &set : sets) {
                set = instance.getCollection(source.size());
            }

            for(auto &set : sets) {
                REQUIRE_THAT(set, Catch::UnorderedEquals(source));
            }
        }
    }
}

SCENARIO("CollectionsProducer: Change source collection")
{
    using namespace aleatoric;

    std::vector<char> source {'a', 'b', 'c'};

    CollectionsProducer<char> instance(
        source,
        NumberProtocol::create(NumberProtocol::Type::cycle));

    // NB: set to reverse bidirectional
    instance.setParams(NumberProtocolParams(CycleParams(true, true)));

    WHEN("New source collection size is too small")
    {
        // Note that in order to have a valid object in the first place, it must
        // have a source with a valid collection size.
        // Therefore when attempting to set the source to one with an
        // invalid collection size, it must mean a change in
        //  size.

        THEN("Throws exception")
        {
            REQUIRE_THROWS_AS(instance.setSource({}), std::invalid_argument);
            REQUIRE_THROWS_WITH(instance.setSource({}),
                                "The size of the source collection provided is "
                                "too small. It must be two or greater");

            REQUIRE_THROWS_AS(instance.setSource({'a'}), std::invalid_argument);
            REQUIRE_THROWS_WITH(instance.setSource({'a'}),
                                "The size of the source collection provided is "
                                "too small. It must be two or greater");

            REQUIRE_NOTHROW(instance.setSource({'a', 'b', 'c'}));
        }

        THEN("The source for the producer should not be updated")
        {
            try {
                instance.setSource({});
            } catch(const std::exception &e) {
                REQUIRE(instance.getSource() == source);
            }
        }
    }

    WHEN("Before source change")
    {
        THEN("Outputs as expected")
        {
            std::vector<char> expectedResult {'c', 'b', 'a', 'b', 'c'};
            auto set = instance.getCollection(expectedResult.size());
            REQUIRE(set == expectedResult);
        }
    }

    WHEN("After source change: no collection size change")
    {
        instance.setSource(std::vector<char> {'x', 'y', 'z'});

        THEN("Output is changed")
        {
            std::vector<char> expectedResult {'z', 'y', 'x', 'y', 'z'};
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

    WHEN("After source change: collection size (and thus range) "
         "change")
    {
        instance.setSource(std::vector<char> {'d', 'e', 'f', 'g'});

        THEN("Output is changed: number protocol is set to default params (due "
             "to range change)")
        {
            // cycle default is: unidirectional, forwards
            std::vector<char>
                expectedResult {'d', 'e', 'f', 'g', 'd', 'e', 'f', 'g'};
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
